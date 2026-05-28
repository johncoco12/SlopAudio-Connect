#pragma once
#include <JuceHeader.h>

class SlopLookAndFeel : public juce::LookAndFeel_V4
{
public:

    static const juce::Colour BG_DEEP;
    static const juce::Colour BG_CARD;
    static const juce::Colour BG_ELEVATED;
    static const juce::Colour BG_HOVER;
    static const juce::Colour ACCENT;
    static const juce::Colour ACCENT_DIM;
    static const juce::Colour TEXT_PRIMARY;
    static const juce::Colour TEXT_MUTED;
    static const juce::Colour BORDER;
    static const juce::Colour BORDER_LIGHT;
    static const juce::Colour SEL_BG;
    static const juce::Colour DANGER;
    static const juce::Colour GOLD;
    static const juce::Colour SUCCESS;
    static const juce::Colour WARNING;

    static const juce::Colour STATE_IDLE;
    static const juce::Colour STATE_DISCOVERING;
    static const juce::Colour STATE_CONNECTING;

    static juce::Colour stateColour(int connectionState);

    static constexpr float FONT_DISPLAY    = 24.0f;
    static constexpr float FONT_HEADING    = 16.0f;
    static constexpr float FONT_SUBHEAD    = 14.0f;
    static constexpr float FONT_BODY       = 14.0f;
    static constexpr float FONT_LABEL      = 12.0f;
    static constexpr float FONT_CAPTION    = 12.0f;
    static constexpr float FONT_MICRO     = 10.0f;
    static constexpr float FONT_NAV       = 14.0f;
    static constexpr float FONT_BTN       = 12.5f;
    static constexpr float FONT_BTN_SM    = 11.0f;

    static constexpr float CORNER         = 8.0f;
    static constexpr float CORNER_SM     = 6.0f;
    static constexpr float CORNER_LG     = 12.0f;
    static constexpr float CORNER_XL     = 16.0f;

    static constexpr int   PAD         = 16;
    static constexpr int   PAD_SM      = 8;
    static constexpr int   PAD_XS      = 6;
    static constexpr int   SECTION_GAP  = 12;
    static constexpr int   BTN_H       = 32;
    static constexpr int   SIDEBAR_W   = 72;
    static constexpr int   STATUS_H    = 30;

    SlopLookAndFeel();

    void drawButtonBackground(juce::Graphics&, juce::Button&,
                               const juce::Colour& bg,
                               bool highlighted, bool down) override;
    void drawButtonText(juce::Graphics&, juce::TextButton&,
                        bool mouseOver, bool buttonDown) override;

    void drawLinearSlider(juce::Graphics&, int x, int y, int w, int h,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          juce::Slider::SliderStyle, juce::Slider&) override;

    void drawTabButton(juce::TabBarButton&, juce::Graphics&,
                       bool isMouseOver, bool isMouseDown) override;
    void drawTabAreaBehindFrontButton(juce::TabbedButtonBar&, juce::Graphics&,
                                      int w, int h) override;

    void fillTextEditorBackground(juce::Graphics&, int w, int h,
                                   juce::TextEditor&) override;
    void drawTextEditorOutline(juce::Graphics&, int w, int h,
                                juce::TextEditor&) override;

    void drawScrollbar(juce::Graphics&, juce::ScrollBar&,
                       int x, int y, int w, int h,
                       bool vertical, int thumbStart, int thumbSize,
                       bool mouseOver, bool mouseDown) override;

    int    getDefaultMenuBarHeight() override { return 0; }

    void   drawDocumentWindowTitleBar(juce::DocumentWindow&,
                                      juce::Graphics&,
                                      int w, int h,
                                      int titleSpaceX, int titleSpaceW,
                                      const juce::Image* icon,
                                      bool drawTitleTextOnLeft) override;

    juce::Button* createDocumentWindowButton(int buttonType) override;

    void   positionDocumentWindowButtons(juce::DocumentWindow&,
                                         int titleBarX, int titleBarY,
                                         int titleBarW, int titleBarH,
                                         juce::Button* minimiseButton,
                                         juce::Button* maximiseButton,
                                         juce::Button* closeButton,
                                         bool positionTitleBarButtonsOnLeft) override;
};