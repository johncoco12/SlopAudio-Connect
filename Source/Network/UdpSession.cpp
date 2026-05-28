#include "UdpSession.h"

UdpSession::UdpSession() : juce::Thread("UdpSession") {}

UdpSession::~UdpSession()
{
    disconnect();
}

void UdpSession::connect(const std::string& ip, int controlPort, int pitchPort,
                         int profileId, const std::string& authToken)
{
    disconnect();

    serverIp          = ip;
    serverControlPort = controlPort;
    serverPitchPort   = pitchPort;
    sessionId         = juce::Uuid().toDashedString().toStdString();
    lastHeartbeatAck  = juce::Time::currentTimeMillis();

    juce::Logger::writeToLog("[UdpSession] connecting to " + juce::String(ip)
        + "  controlPort=" + juce::String(controlPort)
        + "  pitchPort="   + juce::String(pitchPort)
        + "  profileId="   + juce::String(profileId)
        + "  sessionId="   + juce::String(sessionId));

    controlSocket = std::make_unique<juce::DatagramSocket>(false);
    if (!controlSocket->bindToPort(kSacPort))
    {
        juce::Logger::writeToLog("[UdpSession] ERROR: failed to bind control socket to port "
            + juce::String(kSacPort)
            + " — is another instance already running?");
        controlSocket.reset();
        return;
    }
    juce::Logger::writeToLog("[UdpSession] control socket bound to local port " + juce::String(kSacPort));

    pitchSocket = std::make_unique<juce::DatagramSocket>(false);
    if (!pitchSocket->bindToPort(0))
        juce::Logger::writeToLog("[UdpSession] WARNING: pitch socket bind failed (ephemeral port)");
    else
        juce::Logger::writeToLog("[UdpSession] pitch socket ready (ephemeral port)");

    const std::string payload = makeConnectRequest(sessionId, profileId, authToken);
    juce::Logger::writeToLog("[UdpSession] sending CONNECT_REQUEST → " + juce::String(ip)
        + ":" + juce::String(controlPort) + "  payload=" + juce::String(payload));
    sendControl(payload);

    startThread();
    startTimer(kHeartbeatIntervalMs);
    juce::Logger::writeToLog("[UdpSession] receive thread started, heartbeat timer armed ("
        + juce::String(kHeartbeatIntervalMs) + " ms)");
}

void UdpSession::disconnect()
{
    stopTimer();
    signalThreadShouldExit();
    if (controlSocket) controlSocket->shutdown();
    if (pitchSocket)   pitchSocket->shutdown();
    stopThread(2000);
    const bool wasConnected = connected.exchange(false);
    if (wasConnected)
        juce::Logger::writeToLog("[UdpSession] disconnected");
    controlSocket.reset();
    pitchSocket.reset();
}

void UdpSession::sendPitch(float frequency, float confidence,
                            int midiNote, const std::string& noteName)
{
    if (!connected.load() || !pitchSocket) return;

    const std::string payload =
        makePitchPacket(sessionId, frequency, confidence, midiNote, noteName);

    pitchSocket->write(serverIp, serverPitchPort,
                       payload.data(), static_cast<int>(payload.size()));
}

void UdpSession::sendMonitoringStarted()
{
    juce::Logger::writeToLog("[UdpSession] sending MONITORING_STARTED");
    sendControl(makeMonitoringStarted(sessionId));
}

void UdpSession::sendMonitoringStopped()
{
    juce::Logger::writeToLog("[UdpSession] sending MONITORING_STOPPED");
    sendControl(makeMonitoringStopped(sessionId));
}

void UdpSession::sendChainState(const std::string& payload)
{
    juce::Logger::writeToLog("[UdpSession] sending CHAIN_STATE (" + juce::String(payload.size()) + " bytes)");
    sendControl(payload);
}

void UdpSession::sendPluginList(const std::string& payload)
{
    juce::Logger::writeToLog("[UdpSession] sending PLUGIN_LIST (" + juce::String(payload.size()) + " bytes)");
    sendControl(payload);
}

void UdpSession::run()
{
    juce::Logger::writeToLog("[UdpSession] receive loop running");
    char buf[4096];

    while (!threadShouldExit())
    {
        if (!controlSocket)
        {
            juce::Thread::sleep(100);
            continue;
        }

        const int ready = controlSocket->waitUntilReady(true, 500);
        if (ready < 0)
        {
            juce::Logger::writeToLog("[UdpSession] socket error in waitUntilReady — stopping");
            break;
        }
        if (ready == 0)
        {

        }
        else
        {
            juce::String senderHost;
            int senderPort = 0;
            const int n = controlSocket->read(buf, sizeof(buf) - 1, false,
                                              senderHost, senderPort);
            if (n > 0)
            {
                buf[n] = '\0';
                const auto nBytes = static_cast<std::size_t>(n);
                juce::Logger::writeToLog("[UdpSession] received " + juce::String(n)
                    + " bytes from " + senderHost + ":" + juce::String(senderPort)
                    + "  raw=" + juce::String(buf, nBytes));
                processInbound(std::string(buf, nBytes));
            }
            else if (n < 0)
            {
                juce::Logger::writeToLog("[UdpSession] socket read error (n=" + juce::String(n) + ")");
            }
        }

        if (connected.load())
        {
            const juce::int64 elapsed =
                juce::Time::currentTimeMillis() - lastHeartbeatAck;
            if (elapsed > kTimeoutMs)
            {
                juce::Logger::writeToLog("[UdpSession] heartbeat timeout — no ACK for "
                    + juce::String(elapsed) + " ms (limit " + juce::String(kTimeoutMs) + " ms)");
                connected.store(false);
                juce::MessageManager::callAsync([this]
                {
                    listeners.call(&Listener::sessionDisconnected, "heartbeat timeout");
                });
            }
        }
    }

    juce::Logger::writeToLog("[UdpSession] receive loop exited");
}

void UdpSession::timerCallback()
{
    if (connected.load())
    {
        juce::Logger::writeToLog("[UdpSession] sending HEARTBEAT");
        sendControl(makeHeartbeat(sessionId));
    }
}

void UdpSession::processInbound(const std::string& json)
{
    auto msg = InboundMsg::parse(json);
    if (!msg)
    {
        juce::Logger::writeToLog("[UdpSession] WARNING: failed to parse inbound JSON: "
            + juce::String(json));
        return;
    }

    juce::Logger::writeToLog("[UdpSession] inbound type=" + juce::String(msg->type)
        + (msg->status.empty()  ? "" : "  status="  + juce::String(msg->status))
        + (msg->reason.empty()  ? "" : "  reason="  + juce::String(msg->reason))
        + (msg->trackId.empty() ? "" : "  trackId=" + juce::String(msg->trackId)));

    if (msg->type == "CONNECT_ACK")
    {
        if (msg->status == "ok")
        {
            connected.store(true);
            lastHeartbeatAck = juce::Time::currentTimeMillis();
            const std::string sid = msg->sessionId.empty() ? sessionId : msg->sessionId;
            juce::Logger::writeToLog("[UdpSession] CONNECT_ACK ok — sessionId=" + juce::String(sid));
            juce::MessageManager::callAsync([this, sid]
            {
                listeners.call(&Listener::sessionConnected, sid);
            });
        }
        else
        {
            juce::Logger::writeToLog("[UdpSession] CONNECT_ACK denied — reason=" + juce::String(msg->reason));
            juce::MessageManager::callAsync([this, reason = msg->reason]
            {
                listeners.call(&Listener::sessionDenied, reason);
            });
        }
        return;
    }

    if (msg->type == "HEARTBEAT_ACK")
    {
        juce::Logger::writeToLog("[UdpSession] HEARTBEAT_ACK received");
        lastHeartbeatAck = juce::Time::currentTimeMillis();
        return;
    }

    if (msg->type == "START_MONITORING")
    {
        juce::Logger::writeToLog("[UdpSession] START_MONITORING trackId=" + juce::String(msg->trackId)
            + "  tuning=" + juce::String(msg->tuning)
            + "  arrangement=" + juce::String(msg->arrangement));
        juce::MessageManager::callAsync([this,
            trackId = msg->trackId,
            tuning  = msg->tuning,
            arr     = msg->arrangement]
        {
            listeners.call(&Listener::startMonitoringRequested,
                           trackId, tuning, arr);
        });
        return;
    }

    if (msg->type == "STOP_MONITORING")
    {
        juce::Logger::writeToLog("[UdpSession] STOP_MONITORING received");
        juce::MessageManager::callAsync([this]
        {
            listeners.call(&Listener::stopMonitoringRequested);
        });
        return;
    }

    if (msg->type == "DISCONNECT")
    {
        juce::Logger::writeToLog("[UdpSession] DISCONNECT received — reason=" + juce::String(msg->reason));
        connected.store(false);
        juce::MessageManager::callAsync([this, reason = msg->reason]
        {
            listeners.call(&Listener::sessionDisconnected, reason);
        });
        return;
    }

    if (msg->type == "REQUEST_CHAIN_STATE")
    {
        juce::Logger::writeToLog("[UdpSession] REQUEST_CHAIN_STATE received");
        juce::MessageManager::callAsync([this]
        {
            listeners.call(&Listener::requestChainStateReceived);
        });
        return;
    }

    if (msg->type == "SET_PARAMETER")
    {
        juce::Logger::writeToLog("[UdpSession] SET_PARAMETER pluginIndex=" + juce::String(msg->pluginIndex)
            + "  parameterIndex=" + juce::String(msg->parameterIndex)
            + "  value=" + juce::String(msg->value));
        const int   pi  = msg->pluginIndex;
        const int   pai = msg->parameterIndex;
        const float v   = msg->value;
        juce::MessageManager::callAsync([this, pi, pai, v]
        {
            listeners.call(&Listener::setParameterReceived, pi, pai, v);
        });
        return;
    }

    if (msg->type == "SET_BYPASS")
    {
        juce::Logger::writeToLog("[UdpSession] SET_BYPASS pluginIndex=" + juce::String(msg->pluginIndex)
            + "  bypassed=" + juce::String(msg->bypassed ? "true" : "false"));
        const int  pi = msg->pluginIndex;
        const bool by = msg->bypassed;
        juce::MessageManager::callAsync([this, pi, by]
        {
            listeners.call(&Listener::setBypassReceived, pi, by);
        });
        return;
    }

    if (msg->type == "MOVE_PLUGIN")
    {
        juce::Logger::writeToLog("[UdpSession] MOVE_PLUGIN from=" + juce::String(msg->fromIndex)
            + "  to=" + juce::String(msg->toIndex));
        const int from = msg->fromIndex;
        const int to   = msg->toIndex;
        juce::MessageManager::callAsync([this, from, to]
        {
            listeners.call(&Listener::movePluginReceived, from, to);
        });
        return;
    }

    if (msg->type == "REMOVE_PLUGIN")
    {
        juce::Logger::writeToLog("[UdpSession] REMOVE_PLUGIN pluginIndex=" + juce::String(msg->pluginIndex));
        const int pi = msg->pluginIndex;
        juce::MessageManager::callAsync([this, pi]
        {
            listeners.call(&Listener::removePluginReceived, pi);
        });
        return;
    }

    if (msg->type == "ADD_PLUGIN")
    {
        juce::Logger::writeToLog("[UdpSession] ADD_PLUGIN pluginId=" + juce::String(msg->pluginId));
        const std::string pid = msg->pluginId;
        juce::MessageManager::callAsync([this, pid]
        {
            listeners.call(&Listener::addPluginReceived, pid);
        });
        return;
    }

    juce::Logger::writeToLog("[UdpSession] WARNING: unhandled message type=" + juce::String(msg->type));
}

void UdpSession::sendControl(const std::string& payload)
{
    juce::ScopedLock sl(sendLock);
    if (!controlSocket)
    {
        juce::Logger::writeToLog("[UdpSession] WARNING: sendControl called but socket is null");
        return;
    }
    const int sent = controlSocket->write(serverIp, serverControlPort,
                                          payload.data(), static_cast<int>(payload.size()));
    if (sent < 0)
        juce::Logger::writeToLog("[UdpSession] WARNING: sendControl write failed (returned "
            + juce::String(sent) + ")");
}
