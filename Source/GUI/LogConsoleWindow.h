#pragma once
#include <JuceHeader.h>

class LogConsoleWindow : public juce::DocumentWindow
{
public:
    LogConsoleWindow();
    void closeButtonPressed() override;
    void appendLine(const juce::String& text);

private:
    juce::TextEditor output;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LogConsoleWindow)
};
