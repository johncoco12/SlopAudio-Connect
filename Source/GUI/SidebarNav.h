#pragma once
#include <JuceHeader.h>
#include <functional>

class AppState;
enum class ConnectionState;

class SidebarNav : public juce::Component,
                   public juce::ChangeListener
{
public:
    static constexpr int N_ITEMS = 5;
    static constexpr int LOGO_H  = 56;
    static constexpr int ITEM_H   = 72;

    std::function<void(int)> onPageChanged;

    SidebarNav(AppState& state);
    ~SidebarNav() override;

    void setCurrentPage(int idx);
    int  getCurrentPage() const noexcept { return currentPage; }

    void paint(juce::Graphics&) override;
    void mouseDown(const juce::MouseEvent&) override;
    void mouseMove(const juce::MouseEvent&) override;
    void mouseExit(const juce::MouseEvent&)  override;
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    int currentPage  = 0;
    int hoveredItem  = -1;

    AppState& appState;

    juce::Rectangle<int> itemBounds(int idx) const noexcept;
    int itemIndexAt(int y) const noexcept;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SidebarNav)
};