#pragma once
#include <JuceHeader.h>
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

class SlopInput : public juce::Component,
                  private juce::TextEditor::Listener
{
public:
    std::function<void()> onReturnKey;
    std::function<void()> onTextChange;

    SlopInput()
    {
        using SL = SlopLookAndFeel;

        editor.setOpaque(false);
        editor.setColour(juce::TextEditor::backgroundColourId,     juce::Colours::transparentBlack);
        editor.setColour(juce::TextEditor::outlineColourId,        juce::Colours::transparentBlack);
        editor.setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        editor.setColour(juce::TextEditor::textColourId,           SL::TEXT_PRIMARY);
        editor.setColour(juce::TextEditor::highlightColourId,      SL::ACCENT_DIM);
        editor.setColour(juce::TextEditor::highlightedTextColourId,SL::TEXT_PRIMARY);
        editor.setFont(juce::Font(SL::FONT_BODY));
        editor.addListener(this);
        addAndMakeVisible(editor);
    }

    void setIcon(const juce::String& lucideIconName)
    {
        iconName = lucideIconName;
        updateIndent();
        repaint();
    }

    void setPlaceholder(const juce::String& text)
    {
        editor.setTextToShowWhenEmpty(text, SlopLookAndFeel::TEXT_MUTED.withAlpha(0.45f));
    }

    void setText(const juce::String& text, bool notify = false)
    {
        editor.setText(text, notify ? juce::sendNotification : juce::dontSendNotification);
    }

    void setNumericOnly(bool numeric, int maxLength = 0)
    {
        if (numeric)
            editor.setInputRestrictions(maxLength, "0123456789");
        else
            editor.setInputRestrictions(maxLength);
    }

    void setPasswordMode(bool on)
    {
        editor.setPasswordCharacter(on ? 0x2022 : 0);
    }

    void setReadOnly(bool readOnly)
    {
        editor.setReadOnly(readOnly);
    }

    juce::String getText()     const { return editor.getText(); }
    int          getIntValue() const { return editor.getText().getIntValue(); }
    double       getDoubleValue() const { return editor.getText().getDoubleValue(); }
    bool         isFocused()   const { return editor.hasKeyboardFocus(true); }

    void grabFocus() { editor.grabKeyboardFocus(); }

    void resized() override
    {
        editor.setBounds(getLocalBounds());
        updateIndent();
    }

    void paint(juce::Graphics& g) override
    {
        using SL = SlopLookAndFeel;
        const auto b       = getLocalBounds().toFloat();
        const bool focused = editor.hasKeyboardFocus(true);

        if (focused)
        {
            g.setColour(SL::ACCENT.withAlpha(0.15f));
            g.fillRoundedRectangle(b.expanded(2.5f), SL::CORNER + 2.0f);
        }

        g.setColour(focused ? SL::BG_ELEVATED.brighter(0.04f) : SL::BG_ELEVATED);
        g.fillRoundedRectangle(b, SL::CORNER);

        g.setColour(focused ? SL::ACCENT.withAlpha(0.75f) : SL::BORDER.withAlpha(0.7f));
        g.drawRoundedRectangle(b.reduced(0.5f), SL::CORNER, 1.0f);

        if (iconName.isNotEmpty())
        {
            const float iconSz = 15.0f;
            const float iconX  = 10.0f;
            const float iconY  = (b.getHeight() - iconSz) * 0.5f;
            LucideIcons::draw(g, iconName, { iconX, iconY, iconSz, iconSz },
                              focused ? SL::ACCENT.withAlpha(0.85f)
                                      : SL::TEXT_MUTED.withAlpha(0.5f));
        }
    }

    void focusOfChildComponentChanged(FocusChangeType) override { repaint(); }

private:
    juce::TextEditor editor;
    juce::String     iconName;

    void updateIndent()
    {
        using SL = SlopLookAndFeel;
        const int leftPad  = iconName.isNotEmpty() ? 32 : 10;
        const int textH    = (int)SL::FONT_BODY;
        const int topInset = (getHeight() - textH) / 2 - 1;
        editor.setIndents(leftPad, topInset > 0 ? topInset : 0);
    }

    void textEditorReturnKeyPressed(juce::TextEditor&) override
    { if (onReturnKey) onReturnKey(); }

    void textEditorTextChanged(juce::TextEditor&) override
    { if (onTextChange) onTextChange(); }

    void textEditorFocusLost(juce::TextEditor&) override { repaint(); }

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SlopInput)
};
