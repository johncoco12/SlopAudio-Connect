#include "AudioSettingsPanel.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

AudioSettingsPanel::AudioSettingsPanel(AudioEngine& eng)
    : engine(eng),
      selector(eng.getDeviceManager(),
               1, 2,
               0, 2,
               false, false, false, false)
{
    using SL = SlopLookAndFeel;

    monitorBtn.setStyle(SlopButton::IconAndText);
    monitorBtn.setIcon("headphones");
    monitorBtn.setText("Monitor Output");
    monitorBtn.setPrimary(engine.isOutputMonitoring());
    monitorBtn.onClick = [this]
    {
        const bool next = !engine.isOutputMonitoring();
        engine.setOutputMonitoring(next);
        monitorBtn.setPrimary(next);
        monitorBtn.setIcon(next ? "headphones" : "headphones-off");
        monitorBtn.setText(next ? "Monitoring On" : "Monitor Output");
    };
    addAndMakeVisible(monitorBtn);

    addAndMakeVisible(selector);
}

void AudioSettingsPanel::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    g.fillAll(SL::BG_CARD);

    auto area   = getLocalBounds().reduced(SL::PAD);
    auto header = area.removeFromTop(36);

    const float iconSize = 20.0f;
    const float iconY    = (36.0f - iconSize) * 0.5f;
    LucideIcons::draw(g, "audio-lines",
                      { (float)area.getX(), iconY + SL::PAD, iconSize, iconSize },
                      SlopLookAndFeel::ACCENT);

    g.setColour(SL::TEXT_PRIMARY);
    g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
    g.drawText("Audio Settings",
               area.getX() + (int)iconSize + 8, SL::PAD,
               area.getWidth() - (int)iconSize - 8, 36,
               juce::Justification::centredLeft);

    area.removeFromTop(SL::PAD_SM);
    g.setColour(SL::BORDER.withAlpha(0.8f));
    g.fillRect(area.getX(), area.getY(), area.getWidth(), 1);

    juce::ignoreUnused(header);
}

void AudioSettingsPanel::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    area.removeFromTop(36 + SL::PAD_SM + 1 + SL::PAD_SM);

    monitorBtn.setBounds(area.removeFromTop(SL::BTN_H));
    area.removeFromTop(SL::PAD_SM);

    selector.setBounds(area);
}
