#include "LogConsoleWindow.h"
#include "SlopLookAndFeel.h"

LogConsoleWindow::LogConsoleWindow()
    : juce::DocumentWindow("SlopAudio Connect \xe2\x80\x94 Log Console",
                           SlopLookAndFeel::BG_DEEP,
                           juce::DocumentWindow::closeButton)
{
    using SL = SlopLookAndFeel;
    output.setMultiLine(true, false);
    output.setReadOnly(true);
    output.setScrollbarsShown(true);
    output.setCaretVisible(false);
    output.setFont(juce::Font(juce::Font::getDefaultMonospacedFontName(), SL::FONT_LABEL, juce::Font::plain));
    output.setColour(juce::TextEditor::backgroundColourId, SL::BG_DEEP);
    output.setColour(juce::TextEditor::textColourId,       SL::TEXT_PRIMARY);
    output.setColour(juce::TextEditor::outlineColourId,    juce::Colours::transparentBlack);

    setContentNonOwned(&output, false);
    setResizable(true, false);
    setSize(720, 400);
    setTopLeftPosition(60, 60);
    setVisible(true);
}

void LogConsoleWindow::closeButtonPressed()
{
    setVisible(false);
}

void LogConsoleWindow::appendLine(const juce::String& text)
{
    output.moveCaretToEnd();
    output.insertTextAtCaret(text + "\n");
}