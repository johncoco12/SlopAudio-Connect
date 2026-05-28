#include "SidebarNav.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"
#include "../App/AppState.h"

namespace {
    struct NavItem { const char* label; const char* icon; const char* iconActive; };
    const NavItem ITEMS[SidebarNav::N_ITEMS] = {
        { "Connect",  "unplug",           "plug"             },
        { "Profile",  "circle-user-round","circle-user-round" },
        { "Plugins",  "puzzle",           "puzzle"           },
        { "Audio",    "audio-lines",      "audio-lines"      },
        { "Detect",   "radar",            "radar"            },
    };

    bool isConnectedState(ConnectionState s)
    {
        return s == ConnectionState::Authenticated
            || s == ConnectionState::Linked
            || s == ConnectionState::Monitoring;
    }
}

SidebarNav::SidebarNav(AppState& s) : appState(s)
{
    setRepaintsOnMouseActivity(false);
    appState.addChangeListener(this);
}

SidebarNav::~SidebarNav()
{
    appState.removeChangeListener(this);
}

void SidebarNav::setCurrentPage(int idx)
{
    if (currentPage != idx)
    {
        currentPage = idx;
        repaint();
    }
}

juce::Rectangle<int> SidebarNav::itemBounds(int idx) const noexcept
{

    return { 0, LOGO_H + idx * ITEM_H, getWidth(), ITEM_H };
}

int SidebarNav::itemIndexAt(int y) const noexcept
{
    if (y < LOGO_H) return -1;
    const int idx = (y - LOGO_H) / ITEM_H;
    return (idx >= 0 && idx < N_ITEMS) ? idx : -1;
}

void SidebarNav::changeListenerCallback(juce::ChangeBroadcaster*)
{
    repaint();
}

void SidebarNav::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    const int W = getWidth();
    const int H = getHeight();

    g.setColour(SL::BG_DEEP);
    g.fillRect(0, 0, W, H);

    g.setColour(SL::BORDER);
    g.fillRect(W - 1, 0, 1, H);

    {
        const float logoAreaH = (float)LOGO_H;

        g.setColour(SL::BG_CARD);
        g.fillRect(0, 0, W, LOGO_H);

        g.setColour(SL::BORDER);
        g.fillRect(0, LOGO_H - 1, W, 1);

        const float iconSz = 28.0f;
        const float iconX = (W - iconSz) * 0.5f;
        const float iconY = (logoAreaH - iconSz) * 0.5f;
        LucideIcons::draw(g, "audio-waveform",
                          { iconX, iconY, iconSz, iconSz },
                          SL::ACCENT);
    }

    const bool connected = isConnectedState(appState.getConnectionState());

    for (int i = 0; i < N_ITEMS; ++i)
    {
        const bool isActive  = (i == currentPage);
        const bool isHov     = (i == hoveredItem && !isActive);
        const auto ib        = itemBounds(i).toFloat();

        if (isActive)
        {

            juce::ColourGradient grad(SL::ACCENT.withAlpha(0.10f), 0.0f, ib.getY(),
                                       SL::ACCENT.withAlpha(0.04f), (float)W, ib.getY(), false);
            g.setGradientFill(grad);
            g.fillRect(ib);

            g.setColour(SL::ACCENT);
            g.fillRoundedRectangle(0.0f, ib.getY() + 10.0f, 3.0f, ib.getHeight() - 20.0f, 1.5f);
        }
        else if (isHov)
        {
            g.setColour(SL::BG_HOVER.withAlpha(0.7f));
            g.fillRect(ib);
        }

        const float iconSz = 22.0f;
        const float iconX  = (W - iconSz) * 0.5f;
        const float iconY  = ib.getY() + 12.0f;

        const char* iconName = (i == 0 && connected) ? "plug" : ITEMS[i].icon;
        const juce::Colour iconCol = isActive  ? SL::ACCENT
                                   : isHov     ? SL::TEXT_PRIMARY.withAlpha(0.8f)
                                               : SL::TEXT_MUTED.withAlpha(0.75f);

        LucideIcons::draw(g, iconName, { iconX, iconY, iconSz, iconSz }, iconCol);

        const juce::Colour labelCol = isActive  ? SL::TEXT_PRIMARY
                                     : isHov     ? SL::TEXT_PRIMARY.withAlpha(0.7f)
                                                 : SL::TEXT_MUTED.withAlpha(0.6f);
        g.setColour(labelCol);
        g.setFont(juce::Font(SL::FONT_MICRO, isActive ? juce::Font::bold : juce::Font::plain));
        g.drawText(ITEMS[i].label,
                   2, (int)(ib.getY() + iconY - ib.getY() + iconSz + 4.0f),
                   W - 4, 12,
                   juce::Justification::centred);

        if (i < N_ITEMS - 1)
        {
            g.setColour(SL::BORDER.withAlpha(0.5f));
            g.fillRect(8, (int)(ib.getBottom()) - 1, W - 16, 1);
        }
    }
}

void SidebarNav::mouseDown(const juce::MouseEvent& e)
{
    const int idx = itemIndexAt(e.y);
    if (idx >= 0 && idx != currentPage)
    {
        currentPage = idx;
        repaint();
        if (onPageChanged)
            onPageChanged(idx);
    }
}

void SidebarNav::mouseMove(const juce::MouseEvent& e)
{
    const int idx = itemIndexAt(e.y);
    if (idx != hoveredItem)
    {
        hoveredItem = idx;
        repaint();
    }
}

void SidebarNav::mouseExit(const juce::MouseEvent&)
{
    if (hoveredItem != -1)
    {
        hoveredItem = -1;
        repaint();
    }
}
