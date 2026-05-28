#pragma once
#include <JuceHeader.h>
#include "AppState.h"
#include <functional>

class ProfileManager
{
public:
    ProfileManager() = default;

    void fetchProfiles(const std::string& serverIp, int httpPort,
                       std::function<void(std::vector<ProfileInfo>)> onComplete,
                       std::function<void(juce::String)> onError);

    static juce::String computeAuthToken(const juce::String& pinCode,
                                         const juce::String& nonce);

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProfileManager)
};
