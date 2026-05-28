#pragma once
#include <JuceHeader.h>
#include "../App/AppState.h"
#include "../App/SessionController.h"
#include "SlopLookAndFeel.h"
#include "SidebarNav.h"
#include "ConnectionPanel.h"
#include "ProfilePanel.h"
#include "AudioSettingsPanel.h"
#include "PluginChainPanel.h"
#include "DetectionPanel.h"
#include "StatusBar.h"

class MainWindow : public juce::DocumentWindow
{
public:
    MainWindow(const juce::String& name,
               AppState& state,
               SessionController& controller);
    void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow)
};

class MainContent : public juce::Component
{
public:
    MainContent(AppState& state, SessionController& controller);
    ~MainContent() override;
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void setPage(int idx);

    SlopLookAndFeel    laf;

    AppState&          appState;
    SessionController& controller;

    SidebarNav         sidebar;

    ConnectionPanel    connectionPanel;
    ProfilePanel       profilePanel;
    AudioSettingsPanel audioPanel;
    PluginChainPanel   pluginPanel;
    DetectionPanel     detectionPanel;
    StatusBar          statusBar;

    int                currentPage = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainContent)
};
