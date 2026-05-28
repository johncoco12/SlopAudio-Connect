#pragma once
#include <JuceHeader.h>
#include <nlohmann/json.hpp>

static constexpr int kBeaconPort  = 54920;
static constexpr int kControlPort = 54921;
static constexpr int kPitchPort   = 54922;
static constexpr int kSacPort     = 54930;

struct BeaconMsg
{
    std::string serverId;
    std::string serverName;
    std::string ip;
    int         httpPort    = 8080;
    int         controlPort = kControlPort;
    int         pitchPort   = kPitchPort;

    static std::optional<BeaconMsg> parse(const std::string& json,
                                          const juce::String& sourceIp)
    {
        try
        {
            auto j = nlohmann::json::parse(json);
            if (j.value("type", "") != "SLOP_BEACON") return std::nullopt;
            BeaconMsg m;
            m.serverId    = j.value("serverId",    "");
            m.serverName  = j.value("serverName",  "SlopSmith");
            m.httpPort    = j.value("httpPort",    8080);
            m.controlPort = j.value("controlPort", kControlPort);
            m.pitchPort   = j.value("pitchPort",   kPitchPort);

            const std::string explicitIp = j.value("serverIp", "");
            m.ip = explicitIp.empty() ? sourceIp.toStdString() : explicitIp;
            return m;
        }
        catch (...) { return std::nullopt; }
    }
};

inline std::string makeConnectRequest(const std::string& sessionId,
                                      int profileId,
                                      const std::string& authToken)
{
    return nlohmann::json {
        { "type",      "CONNECT_REQUEST" },
        { "sessionId", sessionId         },
        { "profileId", profileId         },
        { "authToken", authToken         },
        { "sacPort",   kSacPort          },
        { "version",   "0.1.0"           }
    }.dump();
}

inline std::string makeHeartbeat(const std::string& sessionId)
{
    return nlohmann::json {
        { "type",      "HEARTBEAT" },
        { "sessionId", sessionId  },
        { "ts",        juce::Time::currentTimeMillis() }
    }.dump();
}

inline std::string makeMonitoringStarted(const std::string& sessionId)
{
    return nlohmann::json {
        { "type",      "MONITORING_STARTED" },
        { "sessionId", sessionId            }
    }.dump();
}

inline std::string makeMonitoringStopped(const std::string& sessionId)
{
    return nlohmann::json {
        { "type",      "MONITORING_STOPPED" },
        { "sessionId", sessionId            }
    }.dump();
}

inline std::string makePitchPacket(const std::string& sessionId,
                                   float frequency,
                                   float confidence,
                                   int   midiNote,
                                   const std::string& noteName)
{
    return nlohmann::json {
        { "type",       "PITCH"     },
        { "sessionId",  sessionId   },
        { "ts",         juce::Time::currentTimeMillis() },
        { "frequency",  frequency   },
        { "confidence", confidence  },
        { "midiNote",   midiNote    },
        { "noteName",   noteName    }
    }.dump();
}

inline std::string makeChainState(const std::string& sessionId,
                                  const nlohmann::json& plugins)
{
    return nlohmann::json {
        { "type",      "CHAIN_STATE" },
        { "sessionId", sessionId     },
        { "plugins",   plugins       }
    }.dump();
}

inline std::string makePluginList(const std::string& sessionId,
                                  const nlohmann::json& plugins)
{
    return nlohmann::json {
        { "type",      "PLUGIN_LIST" },
        { "sessionId", sessionId     },
        { "plugins",   plugins       }
    }.dump();
}

struct InboundMsg
{
    std::string type;
    std::string sessionId;
    std::string status;
    std::string reason;
    std::string trackId;
    std::string tuning;
    std::string arrangement;

    int         pluginIndex    = -1;
    int         parameterIndex = -1;
    float       value          = 0.0f;
    int         fromIndex      = -1;
    int         toIndex        = -1;
    std::string pluginId;
    bool        bypassed       = false;

    static std::optional<InboundMsg> parse(const std::string& json)
    {
        try
        {
            auto j = nlohmann::json::parse(json);
            InboundMsg m;
            m.type        = j.value("type",        "");
            m.sessionId   = j.value("sessionId",   "");
            m.status      = j.value("status",      "");
            m.reason      = j.value("reason",      "");
            m.trackId     = j.value("trackId",     "");

            if (j.contains("tuning"))
                m.tuning = j["tuning"].is_string() ? j["tuning"].get<std::string>()
                                                    : j["tuning"].dump();
            m.arrangement = j.value("arrangement", "");

            m.pluginIndex    = j.value("pluginIndex",    -1);
            m.parameterIndex = j.value("parameterIndex", -1);
            m.value          = j.value("value",          0.0f);
            m.fromIndex      = j.value("fromIndex",      -1);
            m.toIndex        = j.value("toIndex",        -1);
            m.pluginId       = j.value("pluginId",       "");
            m.bypassed       = j.value("bypassed",       false);

            return m;
        }
        catch (...) { return std::nullopt; }
    }
};
