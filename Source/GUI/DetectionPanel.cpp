#include "DetectionPanel.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

juce::PropertiesFile* DetectionPanel::getProps()
{
    static juce::ApplicationProperties appProps;
    static bool initialised = false;
    if (!initialised)
    {
        juce::PropertiesFile::Options opts;
        opts.applicationName     = "SlopAudioConnect";
        opts.filenameSuffix      = ".xml";
        opts.osxLibrarySubFolder = "Application Support";
        appProps.setStorageParameters(opts);
        initialised = true;
    }
    return appProps.getUserSettings();
}

DetectionPanel::DetectionPanel(AudioEngine& e) : engine(e)
{
    using SL = SlopLookAndFeel;

    buildSlider(thresholdSlider, thresholdLabel,
                "Threshold",      0.05, 0.40,   0.005, 0.10);
    buildSlider(minConfSlider,   minConfLabel,
                "Min Confidence", 0.0,  1.0,    0.05,  0.50);
    buildSlider(minFreqSlider,   minFreqLabel,
                "Min Freq",      20.0,  500.0,  1.0,  40.0, " Hz");
    buildSlider(maxFreqSlider,   maxFreqLabel,
                "Max Freq",     200.0, 5000.0, 10.0, 2000.0, " Hz");

    presetsLabel.setText("PRESETS", juce::dontSendNotification);
    presetsLabel.setFont(juce::Font(SL::FONT_MICRO, juce::Font::bold));
    presetsLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    addAndMakeVisible(presetsLabel);

    presetGuitar.setStyle(SlopButton::IconAndText);
    presetGuitar.setIcon("guitar");
    presetGuitar.setText("Guitar");
    presetGuitar.onClick = [this] { applyPreset(0.15f, 0.60f, 70.0f, 1500.0f); };
    addAndMakeVisible(presetGuitar);

    presetBass.setStyle(SlopButton::IconAndText);
    presetBass.setIcon("music-4");
    presetBass.setText("Bass");
    presetBass.onClick = [this] { applyPreset(0.15f, 0.60f, 30.0f, 400.0f); };
    addAndMakeVisible(presetBass);

    presetReset.setStyle(SlopButton::IconAndText);
    presetReset.setIcon("rotate-ccw");
    presetReset.setText("Reset");
    presetReset.onClick = [this] { applyPreset(0.10f, 0.50f, 40.0f, 2000.0f); };
    addAndMakeVisible(presetReset);

    loadSettings();
}

DetectionPanel::~DetectionPanel()
{
    saveSettings();
}

void DetectionPanel::buildSlider(juce::Slider& s, juce::Label& lbl,
                                  const juce::String& name,
                                  double min, double max, double step, double value,
                                  const juce::String& suffix)
{
    using SL = SlopLookAndFeel;
    lbl.setText(name, juce::dontSendNotification);
    lbl.setFont(juce::Font(SL::FONT_LABEL, juce::Font::bold));
    lbl.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    addAndMakeVisible(lbl);

    s.setSliderStyle(juce::Slider::LinearHorizontal);
    s.setTextBoxStyle(juce::Slider::TextBoxRight, false, 72, 22);
    s.setRange(min, max, step);
    s.setValue(value, juce::dontSendNotification);
    if (suffix.isNotEmpty()) s.setTextValueSuffix(suffix);
    s.addListener(this);
    addAndMakeVisible(s);
}

void DetectionPanel::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    g.fillAll(SL::BG_CARD);

    auto area   = getLocalBounds().reduced(SL::PAD);
    auto header = area.removeFromTop(36);

    const float iconSize = 20.0f;
    const float iconY    = (36.0f - iconSize) * 0.5f;
    LucideIcons::draw(g, "radar",
                      { (float)area.getX(), iconY + SL::PAD, iconSize, iconSize },
                      SL::ACCENT);

    g.setColour(SL::TEXT_PRIMARY);
    g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
    g.drawText("Pitch Detection",
               area.getX() + (int)iconSize + 8, SL::PAD,
               area.getWidth() - (int)iconSize - 8, 36,
               juce::Justification::centredLeft);

    area.removeFromTop(SL::PAD_SM);

    g.setColour(SL::BORDER.withAlpha(0.8f));
    g.fillRect(area.getX(), area.getY(), area.getWidth(), 1);

    juce::ignoreUnused(header);
}

void DetectionPanel::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    area.removeFromTop(36 + SL::PAD_SM + 1 + SL::PAD_SM);

    auto sliderRow = [&](juce::Label& lbl, juce::Slider& sl)
    {

        auto labelR = area.removeFromTop(14);
        lbl.setBounds(labelR);
        area.removeFromTop(4);

        sl.setBounds(area.removeFromTop(28));
        area.removeFromTop(SL::PAD_SM + 2);
    };

    sliderRow(thresholdLabel, thresholdSlider);
    sliderRow(minConfLabel,   minConfSlider);
    sliderRow(minFreqLabel,   minFreqSlider);
    sliderRow(maxFreqLabel,   maxFreqSlider);

    area.removeFromTop(SL::SECTION_GAP);
    presetsLabel.setBounds(area.removeFromTop(14));
    area.removeFromTop(SL::PAD_SM);

    auto btnRow = area.removeFromTop(SL::BTN_H);
    const int btnW = (btnRow.getWidth() - SL::PAD_XS * 2) / 3;
    presetGuitar.setBounds(btnRow.removeFromLeft(btnW));
    btnRow.removeFromLeft(SL::PAD_XS);
    presetBass  .setBounds(btnRow.removeFromLeft(btnW));
    btnRow.removeFromLeft(SL::PAD_XS);
    presetReset .setBounds(btnRow.removeFromLeft(btnW));
}

void DetectionPanel::sliderValueChanged(juce::Slider* s)
{
    if (s == &thresholdSlider) engine.setYinThreshold (static_cast<float>(s->getValue()));
    if (s == &minConfSlider)   engine.setMinConfidence(static_cast<float>(s->getValue()));
    if (s == &minFreqSlider)   engine.setMinFreq      (static_cast<float>(s->getValue()));
    if (s == &maxFreqSlider)   engine.setMaxFreq      (static_cast<float>(s->getValue()));
    saveSettings();
}

void DetectionPanel::applyPreset(float threshold, float minConf,
                                  float minFreq, float maxFreq)
{
    thresholdSlider.setValue(threshold, juce::sendNotification);
    minConfSlider  .setValue(minConf,   juce::sendNotification);
    minFreqSlider  .setValue(minFreq,   juce::sendNotification);
    maxFreqSlider  .setValue(maxFreq,   juce::sendNotification);
}

void DetectionPanel::saveSettings()
{
    auto* p = getProps();
    if (!p) return;
    p->setValue("yin.threshold", (double)thresholdSlider.getValue());
    p->setValue("yin.minConf",   (double)minConfSlider  .getValue());
    p->setValue("yin.minFreq",   (double)minFreqSlider  .getValue());
    p->setValue("yin.maxFreq",   (double)maxFreqSlider  .getValue());
    p->saveIfNeeded();
}

void DetectionPanel::loadSettings()
{
    auto* p = getProps();

    const double threshold = p ? p->getDoubleValue("yin.threshold",   0.10) : 0.10;
    const double minConf   = p ? p->getDoubleValue("yin.minConf",     0.50) : 0.50;
    const double minFreq   = p ? p->getDoubleValue("yin.minFreq",    40.0)  : 40.0;
    const double maxFreq   = p ? p->getDoubleValue("yin.maxFreq",  2000.0)  : 2000.0;

    thresholdSlider.setValue(threshold, juce::sendNotification);
    minConfSlider  .setValue(minConf,   juce::sendNotification);
    minFreqSlider  .setValue(minFreq,   juce::sendNotification);
    maxFreqSlider  .setValue(maxFreq,   juce::sendNotification);
}
