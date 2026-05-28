#include "StatusBar.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

StatusBar::StatusBar(AppState& s) : appState(s)
{
    appState.addChangeListener(this);
}

StatusBar::~StatusBar()
{
    appState.removeChangeListener(this);
}

void StatusBar::changeListenerCallback(juce::ChangeBroadcaster*)
{
    repaint();
}

void StatusBar::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    const int W = getWidth();
    const int H = getHeight();

    g.setColour(SL::BG_DEEP);
    g.fillRect(0, 0, W, H);

    g.setColour(SL::BORDER.withAlpha(0.9f));
    g.fillRect(0, 0, W, 1);

    const ConnectionState cs   = appState.getConnectionState();
    const float           freq = appState.getLastFrequency();
    const juce::String    note = appState.getLastNote();
    const float           conf = appState.getLastConfidence();

    const juce::Colour stateCol  = SL::stateColour(static_cast<int>(cs));
    const juce::String stateText = toString(cs);

    {

        const float dotR  = 4.0f;
        const float dotX  = 12.0f;
        const float dotY  = H * 0.5f;

        g.setColour(stateCol.withAlpha(0.18f));
        g.fillEllipse(dotX - dotR - 3.0f, dotY - dotR - 3.0f,
                      (dotR + 3.0f) * 2.0f, (dotR + 3.0f) * 2.0f);

        g.setColour(stateCol);
        g.fillEllipse(dotX - dotR, dotY - dotR, dotR * 2.0f, dotR * 2.0f);

        juce::Font pillFont(SL::FONT_LABEL, juce::Font::bold);
        const float pillW = pillFont.getStringWidthFloat(stateText) + 16.0f;
        const float pillH = 18.0f;
        const float pillX = dotX + dotR + 6.0f;
        const float pillY = (H - pillH) * 0.5f;

        g.setColour(stateCol.withAlpha(0.12f));
        g.fillRoundedRectangle(pillX, pillY, pillW, pillH, pillH * 0.5f);
        g.setColour(stateCol.withAlpha(0.4f));
        g.drawRoundedRectangle(pillX + 0.5f, pillY + 0.5f,
                               pillW - 1.0f, pillH - 1.0f, (pillH - 1.0f) * 0.5f, 1.0f);
        g.setColour(stateCol);
        g.setFont(pillFont);
        g.drawText(stateText,
                   (int)pillX, (int)pillY, (int)pillW, (int)pillH,
                   juce::Justification::centred);
    }

    if (cs == ConnectionState::Monitoring && freq > 0.0f)
    {

        const float noteIconSz = 14.0f;
        const juce::String freqStr = juce::String(freq, 1) + " Hz";

        g.setColour(SL::TEXT_MUTED);
        g.setFont(juce::Font(SL::FONT_CAPTION));
        const float freqW = juce::Font(SL::FONT_CAPTION).getStringWidthFloat(freqStr) + 4.0f;
        g.drawText(freqStr,
                   W - (int)freqW - 10, 0, (int)freqW, H,
                   juce::Justification::centredLeft);

        LucideIcons::draw(g, "music",
                          { (float)(W - (int)freqW - 10 - noteIconSz - 6),
                            (H - noteIconSz) * 0.5f,
                            noteIconSz, noteIconSz },
                          SL::TEXT_MUTED.withAlpha(0.6f));

        g.setColour(SL::TEXT_PRIMARY);
        g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
        const float noteW = juce::Font(SL::FONT_HEADING, juce::Font::bold).getStringWidthFloat(note) + 8.0f;
        g.drawText(note,
                   W - (int)freqW - (int)noteW - (int)noteIconSz - 20, 0,
                   (int)noteW, H,
                   juce::Justification::centredLeft);
    }

    if (cs == ConnectionState::Monitoring)
    {
        const float barH = 2.0f;
        g.setColour(SL::BORDER);
        g.fillRect(0.0f, (float)(H - (int)barH), (float)W, barH);

        const float barW = (float)W * juce::jlimit(0.0f, 1.0f, conf);
        g.setColour(stateCol.withAlpha(0.80f));
        g.fillRect(0.0f, (float)(H - (int)barH), barW, barH);
    }
}
