#pragma once
#include <JuceHeader.h>
#include "AppState.h"
#include "ProfileManager.h"
#include "../Audio/AudioEngine.h"
#include "../Network/BeaconScanner.h"
#include "../Network/UdpSession.h"

class SessionController : private BeaconScanner::Listener,
                           private UdpSession::Listener,
                           private juce::Timer
{
public:
    SessionController(AppState& state);
    ~SessionController() override;

    AudioEngine& getAudioEngine() { return audioEngine; }
    BeaconScanner& getBeaconScanner() { return beaconScanner; }

    void startDiscovery();
    void stopDiscovery();

    void connectToServer(const ServerInfo& server,
                         int profileId,
                         const juce::String& pinCode);

    void disconnect();

    void drainPitchFifo();

private:

    void serverDiscovered(const BeaconMsg& server) override;
    void serverLost(const std::string& serverId) override;

    void sessionConnected(const std::string& sessionId) override;
    void sessionDenied(const std::string& reason) override;
    void sessionDisconnected(const std::string& reason) override;
    void startMonitoringRequested(const std::string& trackId,
                                  const std::string& tuning,
                                  const std::string& arrangement) override;
    void stopMonitoringRequested() override;

    void requestChainStateReceived()                                            override;
    void setParameterReceived(int pluginIndex, int parameterIndex, float value) override;
    void setBypassReceived(int pluginIndex, bool bypassed)                      override;
    void movePluginReceived(int fromIndex, int toIndex)                         override;
    void removePluginReceived(int pluginIndex)                                  override;
    void addPluginReceived(const std::string& pluginId)                         override;

    void broadcastChainState();
    void broadcastPluginList();

    void timerCallback() override;

    AppState&        appState;
    AudioEngine      audioEngine;
    BeaconScanner    beaconScanner;
    UdpSession       udpSession;
    ProfileManager   profileManager;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SessionController)
};
