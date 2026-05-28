#include "SessionController.h"

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
