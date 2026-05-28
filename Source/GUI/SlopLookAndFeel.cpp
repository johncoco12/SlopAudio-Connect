#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

const juce::Colour SlopLookAndFeel::BG_DEEP      { 0xff111111u };
const juce::Colour SlopLookAndFeel::BG_CARD      { 0xff161616u };
const juce::Colour SlopLookAndFeel::BG_ELEVATED  { 0xff1e1e1eu };
const juce::Colour SlopLookAndFeel::BG_HOVER     { 0xff282828u };
const juce::Colour SlopLookAndFeel::ACCENT        { 0xff4080e0u };
const juce::Colour SlopLookAndFeel::ACCENT_DIM    { 0xff182130u };
const juce::Colour SlopLookAndFeel::TEXT_PRIMARY   { 0xffe5e7ebu };
const juce::Colour SlopLookAndFeel::TEXT_MUTED     { 0xff9ca3afu };
const juce::Colour SlopLookAndFeel::BORDER          { 0xff2a2a2au };
const juce::Colour SlopLookAndFeel::BORDER_LIGHT   { 0xff383838u };
const juce::Colour SlopLookAndFeel::SEL_BG          { 0xff192436u };
const juce::Colour SlopLookAndFeel::DANGER           { 0xfff87171u };
const juce::Colour SlopLookAndFeel::GOLD             { 0xffe8c040u };
const juce::Colour SlopLookAndFeel::SUCCESS          { 0xff4ade80u };
const juce::Colour SlopLookAndFeel::WARNING           { 0xfff97316u };

const juce::Colour SlopLookAndFeel::STATE_IDLE         { 0xff5a6080u };
const juce::Colour SlopLookAndFeel::STATE_DISCOVERING   { 0xfffbbf24u };
const juce::Colour SlopLookAndFeel::STATE_CONNECTING    { 0xfff97316u };

juce::Colour SlopLookAndFeel::stateColour(int s)
{
    switch (s)
    {
        case 0:  return STATE_IDLE;
        case 1:  return STATE_DISCOVERING;
        case 2:  return STATE_CONNECTING;
        case 3:  return ACCENT;
        case 4:  return ACCENT;
        case 5:  return ACCENT.brighter(0.2f);
        default: return TEXT_MUTED;
    }
}

SlopLookAndFeel::SlopLookAndFeel()
{
    setColour(juce::ResizableWindow::backgroundColourId, BG_DEEP);

    setColour(juce::TextButton::buttonColourId,         BG_ELEVATED);
    setColour(juce::TextButton::buttonOnColourId,       ACCENT);
    setColour(juce::TextButton::textColourOffId,        TEXT_PRIMARY);
    setColour(juce::TextButton::textColourOnId,         juce::Colours::white);

    setColour(juce::Label::textColourId,                TEXT_PRIMARY);
    setColour(juce::Label::backgroundColourId,          juce::Colours::transparentBlack);

    setColour(juce::ListBox::backgroundColourId,        BG_CARD);
    setColour(juce::ListBox::outlineColourId,           BORDER);

    setColour(juce::TextEditor::backgroundColourId,         BG_ELEVATED);
    setColour(juce::TextEditor::textColourId,               TEXT_PRIMARY);
    setColour(juce::TextEditor::outlineColourId,            BORDER);
    setColour(juce::TextEditor::focusedOutlineColourId,     ACCENT);
    setColour(juce::TextEditor::highlightColourId,          ACCENT_DIM);
    setColour(juce::TextEditor::highlightedTextColourId,    TEXT_PRIMARY);
    setColour(juce::CaretComponent::caretColourId,          ACCENT);

    setColour(juce::Slider::backgroundColourId,             BORDER);
    setColour(juce::Slider::trackColourId,                  ACCENT);
    setColour(juce::Slider::thumbColourId,                  juce::Colours::white);
    setColour(juce::Slider::textBoxTextColourId,            TEXT_MUTED);
    setColour(juce::Slider::textBoxBackgroundColourId,      BG_CARD);
    setColour(juce::Slider::textBoxOutlineColourId,         juce::Colours::transparentBlack);
    setColour(juce::Slider::textBoxHighlightColourId,       ACCENT_DIM);

    setColour(juce::TabbedComponent::outlineColourId,       juce::Colours::transparentBlack);
    setColour(juce::TabbedComponent::backgroundColourId,    BG_DEEP);

    setColour(juce::ScrollBar::backgroundColourId,          juce::Colours::transparentBlack);
    setColour(juce::ScrollBar::thumbColourId,               BORDER);
    setColour(juce::ScrollBar::trackColourId,               juce::Colours::transparentBlack);

    setColour(juce::PopupMenu::backgroundColourId,              BG_CARD);
    setColour(juce::PopupMenu::textColourId,                    TEXT_PRIMARY);
    setColour(juce::PopupMenu::highlightedBackgroundColourId,   SEL_BG);
    setColour(juce::PopupMenu::highlightedTextColourId,         TEXT_PRIMARY);
    setColour(juce::PopupMenu::headerTextColourId,              TEXT_MUTED);

    setColour(juce::AlertWindow::backgroundColourId,    BG_CARD);
    setColour(juce::AlertWindow::textColourId,          TEXT_PRIMARY);
    setColour(juce::AlertWindow::outlineColourId,       BORDER);

    setColour(juce::ComboBox::backgroundColourId,       BG_ELEVATED);
    setColour(juce::ComboBox::textColourId,             TEXT_PRIMARY);
    setColour(juce::ComboBox::outlineColourId,          BORDER);
    setColour(juce::ComboBox::buttonColourId,           BG_ELEVATED);
    setColour(juce::ComboBox::arrowColourId,            TEXT_MUTED);

    setColour(juce::ToggleButton::textColourId,         TEXT_PRIMARY);
    setColour(juce::ToggleButton::tickColourId,         ACCENT);
    setColour(juce::ToggleButton::tickDisabledColourId, TEXT_MUTED);
}

void SlopLookAndFeel::drawButtonBackground(juce::Graphics& g, juce::Button& button,
                                            const juce::Colour& bgColour,
                                            bool highlighted, bool down)
{
    const auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);

    const bool isPrimary = (bgColour == ACCENT);
    const bool enabled   = button.isEnabled();

    juce::Colour base;
    if (isPrimary)
        base = down ? ACCENT.darker(0.25f)
                    : (highlighted ? ACCENT.brighter(0.12f) : ACCENT);
    else
        base = down ? BG_ELEVATED.darker(0.15f)
                    : (highlighted ? BG_HOVER : BG_ELEVATED);

    if (!enabled) base = base.withAlpha(0.45f);

    juce::ColourGradient grad(base.brighter(0.06f), 0.0f, bounds.getY(),
                               base.darker(0.06f),  0.0f, bounds.getBottom(), false);
    g.setGradientFill(grad);
    g.fillRoundedRectangle(bounds, CORNER);

    const juce::Colour border = isPrimary
        ? ACCENT.brighter(0.4f).withAlpha(0.4f)
        : (highlighted ? BORDER_LIGHT : BORDER).withAlpha(enabled ? 0.9f : 0.4f);
    g.setColour(border);
    g.drawRoundedRectangle(bounds, CORNER, 1.0f);
}

void SlopLookAndFeel::drawButtonText(juce::Graphics& g, juce::TextButton& button,
                                      bool  , bool  )
{
    const bool isPrimary = (button.findColour(juce::TextButton::buttonColourId) == ACCENT);
    const bool enabled   = button.isEnabled();

    juce::Colour textCol = isPrimary ? juce::Colours::white : TEXT_PRIMARY;
    if (!enabled) textCol = textCol.withAlpha(0.4f);

    g.setColour(textCol);
    g.setFont(juce::Font(FONT_BTN, juce::Font::bold));
    g.drawFittedText(button.getButtonText(),
                     button.getLocalBounds().reduced(4, 0),
                     juce::Justification::centred, 1);
}

void SlopLookAndFeel::drawLinearSlider(juce::Graphics& g,
                                        int x, int y, int w, int h,
                                        float sliderPos,
                                        float  ,
                                        float  ,
                                        const juce::Slider::SliderStyle style,
                                        juce::Slider& slider)
{
    const bool isHoriz = (style == juce::Slider::LinearHorizontal);
    const bool isVert  = (style == juce::Slider::LinearVertical);

    if (!isHoriz && !isVert)
    {
        juce::LookAndFeel_V4::drawLinearSlider(g, x, y, w, h,
            sliderPos, 0.0f, 0.0f, style, slider);
        return;
    }

    constexpr float TRACK_T = 4.0f;
    constexpr float THUMB_R = 7.0f;

    if (isHoriz)
    {
        const float trackY   = y + (h - TRACK_T) * 0.5f;
        const float fillW    = sliderPos - x;

        g.setColour(BORDER.brighter(0.4f));
        g.fillRoundedRectangle((float)x, trackY, (float)w, TRACK_T, TRACK_T * 0.5f);

        if (fillW > 0.0f)
        {
            g.setColour(ACCENT);
            g.fillRoundedRectangle((float)x, trackY, fillW, TRACK_T, TRACK_T * 0.5f);
        }

        const float tx = sliderPos - THUMB_R;
        const float ty = y + (h - THUMB_R * 2.0f) * 0.5f;
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillEllipse(tx + 1.0f, ty + 1.5f, THUMB_R * 2.0f, THUMB_R * 2.0f);

        g.setColour(slider.isEnabled() ? juce::Colours::white : TEXT_MUTED);
        g.fillEllipse(tx, ty, THUMB_R * 2.0f, THUMB_R * 2.0f);

        if (slider.isMouseButtonDown() || slider.hasKeyboardFocus(true))
        {
            g.setColour(ACCENT.withAlpha(0.45f));
            g.drawEllipse(tx - 3.0f, ty - 3.0f,
                          THUMB_R * 2.0f + 6.0f, THUMB_R * 2.0f + 6.0f, 1.5f);
        }
    }
    else
    {
        const float trackX  = x + (w - TRACK_T) * 0.5f;
        const float fillH   = (float)(y + h) - sliderPos;

        g.setColour(BORDER.brighter(0.4f));
        g.fillRoundedRectangle(trackX, (float)y, TRACK_T, (float)h, TRACK_T * 0.5f);

        if (fillH > 0.0f)
        {
            g.setColour(ACCENT);
            g.fillRoundedRectangle(trackX, sliderPos, TRACK_T, fillH, TRACK_T * 0.5f);
        }

        const float tx = x + (w - THUMB_R * 2.0f) * 0.5f;
        const float ty = sliderPos - THUMB_R;
        g.setColour(juce::Colours::black.withAlpha(0.35f));
        g.fillEllipse(tx + 1.0f, ty + 1.5f, THUMB_R * 2.0f, THUMB_R * 2.0f);
        g.setColour(slider.isEnabled() ? juce::Colours::white : TEXT_MUTED);
        g.fillEllipse(tx, ty, THUMB_R * 2.0f, THUMB_R * 2.0f);
    }
}

void SlopLookAndFeel::drawTabButton(juce::TabBarButton& button, juce::Graphics& g,
                                     bool isMouseOver, bool  )
{
    const bool isActive = button.isFrontTab();
    const auto b = button.getLocalBounds();

    g.setColour(isActive ? BG_CARD : BG_DEEP);
    g.fillRect(b);

    if (isMouseOver && !isActive)
    {
        g.setColour(juce::Colour(0x0fffffff));
        g.fillRect(b);
    }

    const juce::Colour textCol = isActive   ? TEXT_PRIMARY
                               : isMouseOver ? TEXT_PRIMARY.withAlpha(0.65f)
                                             : TEXT_MUTED;
    g.setColour(textCol);
    g.setFont(juce::Font(FONT_LABEL, isActive ? juce::Font::bold : juce::Font::plain));
    g.drawFittedText(button.getButtonText(),
                     b.reduced(4, 0), juce::Justification::centred, 1);

    if (isActive)
    {
        g.setColour(ACCENT);
        g.fillRect((float)b.getX() + 5.0f,
                   (float)b.getBottom() - 2.0f,
                   (float)b.getWidth() - 10.0f, 2.0f);
    }
}

void SlopLookAndFeel::drawTabAreaBehindFrontButton(juce::TabbedButtonBar&  ,
                                                     juce::Graphics& g, int w, int h)
{
    g.setColour(BG_DEEP);
    g.fillRect(0, 0, w, h);

    g.setColour(BORDER);
    g.fillRect(0, h - 1, w, 1);
}

void SlopLookAndFeel::fillTextEditorBackground(juce::Graphics& g, int w, int h,
                                                juce::TextEditor&  )
{
    g.setColour(BG_ELEVATED);
    g.fillRoundedRectangle(0.0f, 0.0f, (float)w, (float)h, CORNER_SM);
}

void SlopLookAndFeel::drawTextEditorOutline(juce::Graphics& g, int w, int h,
                                             juce::TextEditor& te)
{
    const juce::Colour border = te.hasKeyboardFocus(true) ? ACCENT : BORDER;
    g.setColour(border.withAlpha(0.85f));
    g.drawRoundedRectangle(0.5f, 0.5f, (float)w - 1.0f, (float)h - 1.0f, CORNER_SM, 1.0f);
}

void SlopLookAndFeel::drawScrollbar(juce::Graphics& g, juce::ScrollBar&  ,
                                     int x, int y, int w, int h,
                                     bool vertical, int thumbStart, int thumbSize,
                                     bool mouseOver, bool  )
{
    if (thumbSize <= 0) return;

    const juce::Colour col = mouseOver ? BORDER_LIGHT : BORDER;

    juce::Rectangle<float> thumb;
    if (vertical)
        thumb = { x + w * 0.25f, (float)(y + thumbStart), w * 0.5f, (float)thumbSize };
    else
        thumb = { (float)(x + thumbStart), y + h * 0.25f, (float)thumbSize, h * 0.5f };

    g.setColour(col);
    g.fillRoundedRectangle(thumb, vertical ? w * 0.25f : h * 0.25f);
}

void SlopLookAndFeel::drawDocumentWindowTitleBar(
    juce::DocumentWindow& window,
    juce::Graphics& g,
    int w, int h,
    int titleSpaceX, int titleSpaceW,
    const juce::Image*  ,
    bool  )
{
    constexpr float RADIUS = 10.0f;

    juce::Path bg;
    bg.addRoundedRectangle(0.0f, 0.0f, (float)w, (float)h + RADIUS,
                            RADIUS, RADIUS,
                            true, true,
                            false, false);
    g.setColour(BG_DEEP);
    g.fillPath(bg);

    g.setColour(BORDER);
    g.fillRect(0, h - 1, w, 1);

    constexpr int ICON_SIZE = 16;
    constexpr int ICON_PAD  = 12;
    const juce::Rectangle<float> iconBounds {
        (float)ICON_PAD,
        (float)(h - ICON_SIZE) * 0.5f,
        (float)ICON_SIZE,
        (float)ICON_SIZE
    };
    LucideIcons::draw(g, "audio-waveform", iconBounds, ACCENT);

    g.setColour(TEXT_PRIMARY);
    g.setFont(juce::Font(FONT_LABEL, juce::Font::bold));
    g.drawFittedText(window.getName(),
                     titleSpaceX, 0, titleSpaceW, h,
                     juce::Justification::centredLeft, 1);
}

namespace
{
    class TitleBarButton : public juce::Button
    {
    public:
        enum class Kind { Close, Minimise, Maximise };

        TitleBarButton(Kind k) : juce::Button({}), kind(k) {}

        void paintButton(juce::Graphics& g, bool over, bool down) override
        {
            using SL = SlopLookAndFeel;

            const auto b  = getLocalBounds().toFloat().reduced(1.0f);
            const float r = b.getHeight() * 0.25f;

            juce::Colour bg;
            switch (kind)
            {
                case Kind::Close:    bg = SL::DANGER;  break;
                case Kind::Minimise: bg = SL::GOLD;    break;
                case Kind::Maximise: bg = SL::SUCCESS; break;
            }
            if (down)       bg = bg.darker(0.3f);
            else if (over)  bg = bg.brighter(0.2f);

            g.setColour(bg.withAlpha(over || down ? 1.0f : 0.65f));
            g.fillRoundedRectangle(b, r);

            if (over || down)
            {
                const juce::Colour ic = juce::Colours::black.withAlpha(0.75f);
                const float pad = b.getWidth() * 0.22f;
                const auto  ib  = b.reduced(pad);
                switch (kind)
                {
                    case Kind::Close:    LucideIcons::draw(g, "x",     ib, ic); break;
                    case Kind::Minimise: LucideIcons::draw(g, "minus", ib, ic); break;
                    case Kind::Maximise: LucideIcons::draw(g, "square",ib, ic); break;
                }
            }
        }

    private:
        Kind kind;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TitleBarButton)
    };
}

juce::Button* SlopLookAndFeel::createDocumentWindowButton(int buttonType)
{
    using BT = juce::DocumentWindow;
    TitleBarButton::Kind k = TitleBarButton::Kind::Close;
    if (buttonType == BT::minimiseButton) k = TitleBarButton::Kind::Minimise;
    if (buttonType == BT::maximiseButton) k = TitleBarButton::Kind::Maximise;
    return new TitleBarButton(k);
}

void SlopLookAndFeel::positionDocumentWindowButtons(
    juce::DocumentWindow&  ,
    int  , int titleBarY,
    int titleBarW, int titleBarH,
    juce::Button* minimiseButton,
    juce::Button* maximiseButton,
    juce::Button* closeButton,
    bool  )
{
    constexpr int BTN_W     = 28;
    constexpr int BTN_H     = 18;
    constexpr int GAP       = 6;
    constexpr int RIGHT_PAD = 12;

    const int y = titleBarY + (titleBarH - BTN_H) / 2;
    int x = titleBarW - RIGHT_PAD - BTN_W;

    auto place = [&](juce::Button* b)
    {
        if (b) { b->setBounds(x, y, BTN_W, BTN_H); x -= (BTN_W + GAP); }
    };

    place(closeButton);
    place(maximiseButton);
    place(minimiseButton);
}