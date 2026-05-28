#pragma once
#include <JuceHeader.h>
#include "../Audio/AudioEngine.h"
#include "SlopButton.h"

class DetectionPanel : public juce::Component,
                       public juce::Slider::Listener
{
public:
    explicit DetectionPanel(AudioEngine& engine);
    ~DetectionPanel() override;

    void resized() override;
    void paint(juce::Graphics& g) override;
    void sliderValueChanged(juce::Slider* slider) override;

private:
    void buildSlider(juce::Slider& s, juce::Label& lbl,
                     const juce::String& name,
                     double min, double max, double step, double value,
                     const juce::String& suffix = {});

    void applyPreset(float threshold, float minConf, float minFreq, float maxFreq);
    void saveSettings();
    void loadSettings();

    static juce::PropertiesFile* getProps();

    AudioEngine& engine;

    juce::Label  thresholdLabel;
    juce::Slider thresholdSlider;

    juce::Label  minConfLabel;
    juce::Slider minConfSlider;

    juce::Label  minFreqLabel;
    juce::Slider minFreqSlider;

    juce::Label  maxFreqLabel;
    juce::Slider maxFreqSlider;

    juce::Label presetsLabel;
    SlopButton  presetGuitar;
    SlopButton  presetBass;
    SlopButton  presetReset;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DetectionPanel)
};
