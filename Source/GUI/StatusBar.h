#pragma once
#include <JuceHeader.h>
#include "../App/AppState.h"

class StatusBar : public juce::Component,
                  public juce::ChangeListener
{
public:
    explicit StatusBar(AppState& state);
    ~StatusBar() override;

    void paint(juce::Graphics& g) override;
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    AppState& appState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(StatusBar)
};
