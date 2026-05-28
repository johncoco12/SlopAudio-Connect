#pragma once
#include <JuceHeader.h>
#include "MessageTypes.h"
#include <atomic>
#include <string>

class UdpSession : private juce::Thread,
                   private juce::Timer
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void sessionConnected(const std::string& sessionId) = 0;
        virtual void sessionDenied(const std::string& reason)       = 0;
        virtual void sessionDisconnected(const std::string& reason) = 0;
        virtual void startMonitoringRequested(const std::string& trackId,
                                              const std::string& tuning,
                                              const std::string& arrangement) = 0;
        virtual void stopMonitoringRequested()                       = 0;
    };

    UdpSession();
    ~UdpSession() override;

    void addListener(Listener* l)    { listeners.add(l); }
    void removeListener(Listener* l) { listeners.remove(l); }

    void connect(const std::string& serverIp, int controlPort, int pitchPort,
                 int profileId, const std::string& authToken);
    void disconnect();

    bool isConnected() const { return connected.load(); }
    const std::string& getSessionId() const { return sessionId; }

    void sendPitch(float frequency, float confidence,
                   int midiNote, const std::string& noteName);

    void sendMonitoringStarted();
    void sendMonitoringStopped();

private:
    void run() override;
    void timerCallback() override;

    void processInbound(const std::string& json);
    void sendControl(const std::string& payload);

    static constexpr int kHeartbeatIntervalMs = 5000;
    static constexpr int kTimeoutMs           = 15000;

    std::unique_ptr<juce::DatagramSocket> controlSocket;
    std::unique_ptr<juce::DatagramSocket> pitchSocket;

    std::string serverIp;
    int         serverControlPort = kControlPort;
    int         serverPitchPort   = kPitchPort;

    std::string sessionId;
    std::atomic<bool> connected { false };

    juce::int64 lastHeartbeatAck { 0 };

    juce::ListenerList<Listener> listeners;
    juce::CriticalSection        sendLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UdpSession)
};
