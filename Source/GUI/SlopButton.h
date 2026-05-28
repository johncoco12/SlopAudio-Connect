#pragma once
#include <JuceHeader.h>
#include "LucideIcons.h"
#include "SlopLookAndFeel.h"

class SlopButton : public juce::Component
{
public:
    enum Style { Text, Icon, IconAndText };

    std::function<void()> onClick;

    SlopButton() = default;

    void setIcon(const juce::String& name)       { iconName = name; repaint(); }
    void setText(const juce::String& t)           { labelText = t;   repaint(); }
    void setStyle(Style s)                         { style = s;       repaint(); }
    void setColour(juce::Colour c)                { accentColour = c; repaint(); }

    void setPrimary(bool v)
    {
        isPrimary = v;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        using SL = SlopLookAndFeel;
        const auto b = getLocalBounds().toFloat().reduced(0.5f);
        const bool down      = isDown;
        const bool hovered    = isHovered;
        const bool enabled    = isEnabled();

        juce::Colour bg;
        if (isPrimary)
            bg = down ? accentColour.darker(0.25f)
                      : (hovered ? accentColour.brighter(0.12f) : accentColour);
        else
            bg = down ? SL::BG_ELEVATED.darker(0.15f)
                      : (hovered ? SL::BG_HOVER : SL::BG_ELEVATED);

        if (!enabled) bg = bg.withAlpha(0.45f);

        juce::ColourGradient grad(bg.brighter(0.06f), 0.0f, b.getY(),
                                   bg.darker(0.06f),  0.0f, b.getBottom(), false);
        g.setGradientFill(grad);
        g.fillRoundedRectangle(b, SL::CORNER);

        const juce::Colour border = isPrimary
            ? accentColour.brighter(0.4f).withAlpha(0.4f)
            : (hovered ? SL::BORDER_LIGHT : SL::BORDER).withAlpha(enabled ? 0.9f : 0.4f);
        g.setColour(border);
        g.drawRoundedRectangle(b, SL::CORNER, 1.0f);

        if (style == Icon)
            paintIcon(g, enabled, hovered);
        else if (style == Text)
            paintText(g, enabled);
        else
        {
            const float iconW = 18.0f;
            paintIcon(g, enabled, hovered, iconW);
            paintText(g, enabled, iconW + 4.0f);
        }
    }

    void resized() override {}

    void mouseDown(const juce::MouseEvent&)  override { isDown = true;  repaint(); }
    void mouseUp(const juce::MouseEvent& e)   override
    {
        isDown = false; repaint();
        if (getLocalBounds().contains(e.getPosition()) && onClick)
            onClick();
    }
    void mouseEnter(const juce::MouseEvent&) override { isHovered = true;  repaint(); }
    void mouseExit(const juce::MouseEvent&)  override { isHovered = false; repaint(); }

    void enablementChanged() override { repaint(); }

private:
    void paintIcon(juce::Graphics& g, bool enabled, bool hovered,
                   float offsetX = 0.0f)
    {
        if (iconName.isEmpty()) return;

        const float sz = (style == IconAndText) ? 16.0f : 18.0f;
        const float x = (style == Icon)
            ? (getWidth()  - sz) * 0.5f
            : (style == IconAndText ? 10.0f + offsetX : 0.0f);
        const float y = (getHeight() - sz) * 0.5f;

        juce::Colour col = isPrimary ? juce::Colours::white
                         : (hovered  ? SlopLookAndFeel::ACCENT
                                     : SlopLookAndFeel::TEXT_MUTED);
        if (!enabled) col = col.withAlpha(0.4f);

        LucideIcons::draw(g, iconName, { x, y, sz, sz }, col);
    }

    void paintText(juce::Graphics& g, bool enabled, float textOffsetX = 0.0f)
    {
        if (labelText.isEmpty()) return;

        juce::Colour col = isPrimary ? juce::Colours::white
                         : SlopLookAndFeel::TEXT_PRIMARY;
        if (!enabled) col = col.withAlpha(0.4f);

        g.setColour(col);
        g.setFont(juce::Font(SlopLookAndFeel::FONT_BTN, juce::Font::bold));

        int leftMargin = 0;
        if (style == IconAndText && !iconName.isEmpty())
            leftMargin = 40;

        g.drawFittedText(labelText,
                         getLocalBounds().reduced(4, 0).withLeft(leftMargin),
                         juce::Justification::centred, 1);
    }

    Style        style       = Text;
    juce::String iconName;
    juce::String labelText;
    juce::Colour accentColour{ SlopLookAndFeel::ACCENT };
    bool         isPrimary   = false;
    bool         isDown      = false;
    bool         isHovered   = false;
};