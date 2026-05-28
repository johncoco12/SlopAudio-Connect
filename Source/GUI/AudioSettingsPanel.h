#pragma once
#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"
#include "SlopButton.h"

class AudioSettingsPanel : public juce::Component
{
public:
    explicit AudioSettingsPanel(AudioEngine& engine);
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    AudioEngine& engine;

    SlopButton                      monitorBtn;
    juce::AudioDeviceSelectorComponent selector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioSettingsPanel)
};
