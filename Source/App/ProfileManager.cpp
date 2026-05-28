#include "ProfileManager.h"
#include <nlohmann/json.hpp>

void ProfileManager::fetchProfiles(const std::string& serverIp,
                                    int httpPort,
                                    std::function<void(std::vector<ProfileInfo>)> onComplete,
                                    std::function<void(juce::String)> onError)
{
    const juce::String url =
        "http://" + juce::String(serverIp) + ":" + juce::String(httpPort) + "/api/profiles";

    juce::Logger::writeToLog("[ProfileManager] fetching profiles from " + url);

    juce::Thread::launch([url, onComplete, onError]
    {
        juce::URL juceUrl(url);
        std::unique_ptr<juce::InputStream> stream(
            juceUrl.createInputStream(juce::URL::InputStreamOptions(
                juce::URL::ParameterHandling::inAddress)));

        if (!stream)
        {
            juce::Logger::writeToLog("[ProfileManager] ERROR: could not open stream to " + url);
            juce::MessageManager::callAsync([onError]
            {
                onError("Could not connect to server");
            });
            return;
        }

        const juce::String body = stream->readEntireStreamAsString();
        juce::Logger::writeToLog("[ProfileManager] response (" + juce::String(body.length())
            + " chars): " + body.substring(0, 300));

        try
        {
            auto j = nlohmann::json::parse(body.toStdString());
            std::vector<ProfileInfo> list;

            for (auto& item : j)
            {
                ProfileInfo p;
                p.id       = item.value("id", 0);
                p.name     = juce::String(item.value("name", std::string{}));
                p.avatarId = (item.contains("avatarId") && item["avatarId"].is_number_integer())
                             ? item["avatarId"].get<int>() : -1;
                list.push_back(p);
                juce::Logger::writeToLog("[ProfileManager] profile: id=" + juce::String(p.id)
                    + "  name=" + p.name);
            }

            juce::Logger::writeToLog("[ProfileManager] loaded " + juce::String((int)list.size()) + " profile(s)");

            juce::MessageManager::callAsync([onComplete, list]
            {
                onComplete(list);
            });
        }
        catch (const std::exception& e)
        {
            juce::Logger::writeToLog("[ProfileManager] ERROR: parse failed — " + juce::String(e.what()));
            juce::MessageManager::callAsync([onError, msg = juce::String(e.what())]
            {
                onError("Parse error: " + msg);
            });
        }
    });
}

juce::String ProfileManager::computeAuthToken(const juce::String& pinCode,
                                               const juce::String&  )
{

    return pinCode;
}
