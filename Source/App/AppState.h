#pragma once
#include <JuceHeader.h>
#include <string>
#include <vector>

enum class ConnectionState
{
    Idle,
    Discovering,
    Connecting,
    Authenticated,
    Linked,
    Monitoring
};

inline const char* toString(ConnectionState s)
{
    switch (s)
    {
        case ConnectionState::Idle:          return "Idle";
        case ConnectionState::Discovering:   return "Discovering";
        case ConnectionState::Connecting:    return "Connecting";
        case ConnectionState::Authenticated: return "Authenticated";
        case ConnectionState::Linked:        return "Linked";
        case ConnectionState::Monitoring:    return "Monitoring";
    }
    return "Unknown";
}

struct ProfileInfo
{
    int          id      = 0;
    juce::String name;
    int          avatarId = -1;
};

struct ServerInfo
{
    std::string serverId;
    std::string serverName;
    std::string ip;
    int         httpPort    = 8080;
    int         controlPort = 54921;
    int         pitchPort   = 54922;
};

class AppState : public juce::ChangeBroadcaster
{
public:
    AppState() = default;

    ConnectionState getConnectionState() const { return connectionState; }
    void            setConnectionState(ConnectionState s);

    const std::vector<ServerInfo>& getServers()  const { return servers; }
    void setServers(std::vector<ServerInfo> list);
    void upsertServer(const ServerInfo& s);
    void removeServer(const std::string& serverId);

    const ServerInfo* getSelectedServer() const { return selectedServer.get(); }
    void setSelectedServer(const ServerInfo& s);
    void clearSelectedServer();

    const std::vector<ProfileInfo>& getProfiles() const { return profiles; }
    void setProfiles(std::vector<ProfileInfo> list);

    const ProfileInfo* getActiveProfile() const { return activeProfile.get(); }
    void setActiveProfile(const ProfileInfo& p);
    void clearActiveProfile();

    const std::string& getSessionId() const { return sessionId; }
    void setSessionId(const std::string& id);

    const std::string& getActiveTrackId() const   { return activeTrackId; }
    void setActiveTrackId(const std::string& id);

    float getLastFrequency()   const { return lastFreq; }
    float getLastConfidence()  const { return lastConf; }
    juce::String getLastNote() const { return lastNote; }
    void setPitchData(float freq, float conf, const juce::String& note);

    juce::String getLastError() const { return lastError; }
    void setLastError(const juce::String& err);

private:
    ConnectionState connectionState = ConnectionState::Idle;
    std::vector<ServerInfo>  servers;
    std::unique_ptr<ServerInfo>  selectedServer;
    std::vector<ProfileInfo> profiles;
    std::unique_ptr<ProfileInfo> activeProfile;
    std::string sessionId;
    std::string activeTrackId;
    float        lastFreq  = 0.0f;
    float        lastConf  = 0.0f;
    juce::String lastNote;
    juce::String lastError;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AppState)
};
