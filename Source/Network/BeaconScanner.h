#pragma once
#include <JuceHeader.h>
#include "MessageTypes.h"
#include <vector>
#include <map>

class BeaconScanner : private juce::Thread
{
public:
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void serverDiscovered(const BeaconMsg& server) = 0;
        virtual void serverLost(const std::string& serverId)   = 0;
    };

    BeaconScanner();
    ~BeaconScanner() override;

    void start();
    void stop();

    void addListener(Listener* l)    { listeners.add(l); }
    void removeListener(Listener* l) { listeners.remove(l); }

    std::vector<BeaconMsg> getServers() const;

private:
    void run() override;
    void expireStaleServers();

    static constexpr int kExpireMs = 8000;

    mutable juce::CriticalSection        serversLock;
    std::map<std::string, BeaconMsg>     servers;
    std::map<std::string, juce::int64>   lastSeen;

    juce::ListenerList<Listener> listeners;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BeaconScanner)
};
