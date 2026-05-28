#include "BeaconScanner.h"

BeaconScanner::BeaconScanner() : juce::Thread("BeaconScanner") {}

BeaconScanner::~BeaconScanner()
{
    stop();
}

void BeaconScanner::start()
{
    startThread();
}

void BeaconScanner::stop()
{
    signalThreadShouldExit();
    stopThread(2000);
}

std::vector<BeaconMsg> BeaconScanner::getServers() const
{
    juce::ScopedLock sl(serversLock);
    std::vector<BeaconMsg> out;
    for (auto& [id, msg] : servers)
        out.push_back(msg);
    return out;
}

void BeaconScanner::run()
{
    juce::DatagramSocket socket(true  );

    if (!socket.bindToPort(kBeaconPort))
    {
        DBG("BeaconScanner: failed to bind to port " << kBeaconPort);
        return;
    }

    char buf[2048];

    while (!threadShouldExit())
    {
        if (!socket.waitUntilReady(true, 500))
        {
            expireStaleServers();
            continue;
        }

        juce::String senderHost;
        int senderPort = 0;
        const int n = socket.read(buf, sizeof(buf) - 1, false,
                                  senderHost, senderPort);
        if (n <= 0)
            continue;

        buf[n] = '\0';
        auto msg = BeaconMsg::parse(std::string(buf, n), senderHost);
        if (!msg)
            continue;

        const juce::int64 now = juce::Time::currentTimeMillis();
        bool isNew = false;

        {
            juce::ScopedLock sl(serversLock);
            isNew         = (servers.find(msg->serverId) == servers.end());
            servers[msg->serverId]  = *msg;
            lastSeen[msg->serverId] = now;
        }

        const BeaconMsg captured = *msg;
        juce::MessageManager::callAsync([this, captured, isNew]
        {
            listeners.call(&Listener::serverDiscovered, captured);
        });

        expireStaleServers();
    }
}

void BeaconScanner::expireStaleServers()
{
    const juce::int64 now = juce::Time::currentTimeMillis();
    std::vector<std::string> expired;

    {
        juce::ScopedLock sl(serversLock);
        for (auto& [id, ts] : lastSeen)
            if (now - ts > kExpireMs)
                expired.push_back(id);

        for (auto& id : expired)
        {
            servers.erase(id);
            lastSeen.erase(id);
        }
    }

    for (auto& id : expired)
    {
        const std::string captured = id;
        juce::MessageManager::callAsync([this, captured]
        {
            listeners.call(&Listener::serverLost, captured);
        });
    }
}
