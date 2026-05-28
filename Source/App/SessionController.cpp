#include "SessionController.h"
#include "../Network/MessageTypes.h"

SessionController::SessionController(AppState& state)
    : appState(state)
{
    audioEngine.initialise();
    beaconScanner.addListener(this);
    udpSession.addListener(this);
}

SessionController::~SessionController()
{
    stopTimer();
    beaconScanner.removeListener(this);
    udpSession.removeListener(this);
    disconnect();
}

void SessionController::startDiscovery()
{
    juce::Logger::writeToLog("[SessionController] starting beacon discovery");
    appState.setConnectionState(ConnectionState::Discovering);
    beaconScanner.start();
}

void SessionController::stopDiscovery()
{
    juce::Logger::writeToLog("[SessionController] stopping beacon discovery");
    beaconScanner.stop();
    if (appState.getConnectionState() == ConnectionState::Discovering)
        appState.setConnectionState(ConnectionState::Idle);
}

void SessionController::connectToServer(const ServerInfo& server,
                                         int profileId,
                                         const juce::String& pinCode)
{
    juce::Logger::writeToLog("[SessionController] connectToServer"
        + juce::String("  server=")  + juce::String(server.serverName)
        + "  ip="          + juce::String(server.ip)
        + "  controlPort=" + juce::String(server.controlPort)
        + "  pitchPort="   + juce::String(server.pitchPort)
        + "  profileId="   + juce::String(profileId));

    appState.setSelectedServer(server);
    appState.setConnectionState(ConnectionState::Connecting);

    const juce::String nonce = juce::Uuid().toDashedString();
    const std::string  token = ProfileManager::computeAuthToken(pinCode, nonce)
                                   .toStdString();

    juce::Logger::writeToLog("[SessionController] auth token computed, opening UDP session");
    udpSession.connect(server.ip, server.controlPort, server.pitchPort,
                       profileId, token);
}

void SessionController::disconnect()
{
    juce::Logger::writeToLog("[SessionController] disconnect");
    stopTimer();
    udpSession.disconnect();
    audioEngine.setMonitoring(false);
    appState.setConnectionState(ConnectionState::Idle);
    appState.clearActiveProfile();
    appState.clearSelectedServer();
}

void SessionController::drainPitchFifo()
{
    AudioEngine::PitchResult pr;
    while (audioEngine.readPitchResult(pr))
    {
        appState.setPitchData(pr.frequency, pr.confidence, pr.noteName);
        udpSession.sendPitch(pr.frequency, pr.confidence,
                             pr.midiNote, pr.noteName.toStdString());
    }
}

void SessionController::serverDiscovered(const BeaconMsg& msg)
{
    juce::Logger::writeToLog("[SessionController] server discovered: "
        + juce::String(msg.serverName)
        + "  id="          + juce::String(msg.serverId)
        + "  ip="          + juce::String(msg.ip)
        + "  controlPort=" + juce::String(msg.controlPort)
        + "  pitchPort="   + juce::String(msg.pitchPort));

    ServerInfo s;
    s.serverId    = msg.serverId;
    s.serverName  = msg.serverName;
    s.ip          = msg.ip;
    s.httpPort    = msg.httpPort;
    s.controlPort = msg.controlPort;
    s.pitchPort   = msg.pitchPort;
    appState.upsertServer(s);
}

void SessionController::serverLost(const std::string& serverId)
{
    juce::Logger::writeToLog("[SessionController] server lost: " + juce::String(serverId));
    appState.removeServer(serverId);
}

void SessionController::sessionConnected(const std::string& sessionId)
{
    juce::Logger::writeToLog("[SessionController] session connected — sessionId=" + juce::String(sessionId));
    appState.setSessionId(sessionId);
    appState.setConnectionState(ConnectionState::Authenticated);
    startTimerHz(60);

    audioEngine.getPluginChain().onChainChanged = [this]()
    {
        broadcastChainState();
    };

    broadcastPluginList();
    broadcastChainState();
}

void SessionController::sessionDenied(const std::string& reason)
{
    juce::Logger::writeToLog("[SessionController] session denied — reason=" + juce::String(reason));
    appState.setLastError("Connection denied: " + juce::String(reason));
    appState.setConnectionState(ConnectionState::Idle);
}

void SessionController::sessionDisconnected(const std::string& reason)
{
    juce::Logger::writeToLog("[SessionController] session disconnected — reason=" + juce::String(reason));
    stopTimer();
    audioEngine.setMonitoring(false);
    audioEngine.getPluginChain().onChainChanged = nullptr;
    appState.setLastError("Disconnected: " + juce::String(reason));
    appState.setConnectionState(ConnectionState::Idle);
}

void SessionController::startMonitoringRequested(const std::string& trackId,
                                                   const std::string& tuning,
                                                   const std::string& arrangement)
{
    juce::Logger::writeToLog("[SessionController] startMonitoring trackId=" + juce::String(trackId)
        + "  tuning=" + juce::String(tuning) + "  arrangement=" + juce::String(arrangement));
    appState.setActiveTrackId(trackId);
    audioEngine.setMonitoring(true);
    udpSession.sendMonitoringStarted();
    appState.setConnectionState(ConnectionState::Monitoring);
}

void SessionController::stopMonitoringRequested()
{
    juce::Logger::writeToLog("[SessionController] stopMonitoring");
    audioEngine.setMonitoring(false);
    udpSession.sendMonitoringStopped();
    appState.setActiveTrackId("");
    appState.setConnectionState(ConnectionState::Linked);
}

void SessionController::timerCallback()
{
    drainPitchFifo();
}

void SessionController::requestChainStateReceived()
{
    juce::Logger::writeToLog("[SessionController] requestChainState received");
    broadcastPluginList();
    broadcastChainState();
}

void SessionController::setParameterReceived(int pluginIndex, int parameterIndex, float value)
{

    audioEngine.getPluginChain().queueParameterChange(pluginIndex, parameterIndex, value);
}

void SessionController::setBypassReceived(int pluginIndex, bool bypassed)
{
    juce::Logger::writeToLog("[SessionController] setBypass pluginIndex=" + juce::String(pluginIndex)
        + "  bypassed=" + juce::String(bypassed ? "true" : "false"));
    audioEngine.getPluginChain().setBypassed(pluginIndex, bypassed);

}

void SessionController::movePluginReceived(int fromIndex, int toIndex)
{
    juce::Logger::writeToLog("[SessionController] movePlugin from=" + juce::String(fromIndex)
        + "  to=" + juce::String(toIndex));
    audioEngine.getPluginChain().movePlugin(fromIndex, toIndex);

}

void SessionController::removePluginReceived(int pluginIndex)
{
    juce::Logger::writeToLog("[SessionController] removePlugin index=" + juce::String(pluginIndex));
    audioEngine.getPluginChain().removePlugin(pluginIndex);

}

void SessionController::addPluginReceived(const std::string& pluginId)
{
    juce::Logger::writeToLog("[SessionController] addPlugin pluginId=" + juce::String(pluginId));

    auto& chain = audioEngine.getPluginChain();
    const auto& known = chain.getKnownPlugins();

    for (const auto& desc : known.getTypes())
    {
        if (desc.createIdentifierString().toStdString() == pluginId)
        {
            juce::String err;
            if (!chain.addPlugin(desc, err))
                juce::Logger::writeToLog("[SessionController] addPlugin failed: " + err);

            return;
        }
    }

    juce::Logger::writeToLog("[SessionController] addPlugin: pluginId not in known list: "
        + juce::String(pluginId));
}

void SessionController::broadcastChainState()
{
    if (!udpSession.isConnected()) return;
    udpSession.sendChainState(
        makeChainState(udpSession.getSessionId(),
                       audioEngine.getPluginChain().serialiseChain()));
}

void SessionController::broadcastPluginList()
{
    if (!udpSession.isConnected()) return;
    udpSession.sendPluginList(
        makePluginList(udpSession.getSessionId(),
                       audioEngine.getPluginChain().serialisePluginList()));
}
