#include "AppState.h"

void AppState::setConnectionState(ConnectionState s)
{
    connectionState = s;
    sendChangeMessage();
}

void AppState::setServers(std::vector<ServerInfo> list)
{
    servers = std::move(list);
    sendChangeMessage();
}

void AppState::upsertServer(const ServerInfo& s)
{
    for (auto& existing : servers)
    {
        if (existing.serverId == s.serverId)
        {
            existing = s;
            sendChangeMessage();
            return;
        }
    }
    servers.push_back(s);
    sendChangeMessage();
}

void AppState::removeServer(const std::string& id)
{
    servers.erase(std::remove_if(servers.begin(), servers.end(),
                                  [&](const ServerInfo& s){ return s.serverId == id; }),
                  servers.end());
    sendChangeMessage();
}

void AppState::setSelectedServer(const ServerInfo& s)
{
    selectedServer = std::make_unique<ServerInfo>(s);
    sendChangeMessage();
}

void AppState::clearSelectedServer()
{
    selectedServer.reset();
    sendChangeMessage();
}

void AppState::setProfiles(std::vector<ProfileInfo> list)
{
    profiles = std::move(list);
    sendChangeMessage();
}

void AppState::setActiveProfile(const ProfileInfo& p)
{
    activeProfile = std::make_unique<ProfileInfo>(p);
    sendChangeMessage();
}

void AppState::clearActiveProfile()
{
    activeProfile.reset();
    sendChangeMessage();
}

void AppState::setSessionId(const std::string& id)
{
    sessionId = id;
    sendChangeMessage();
}

void AppState::setActiveTrackId(const std::string& id)
{
    activeTrackId = id;
    sendChangeMessage();
}

void AppState::setPitchData(float freq, float conf, const juce::String& note)
{
    lastFreq = freq;
    lastConf = conf;
    lastNote = note;
    sendChangeMessage();
}

void AppState::setLastError(const juce::String& err)
{
    lastError = err;
    sendChangeMessage();
}
