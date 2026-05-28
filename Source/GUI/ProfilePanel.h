#pragma once
#include <JuceHeader.h>
#include "../App/AppState.h"
#include "../App/SessionController.h"
#include "SlopButton.h"
#include "SlopInput.h"

class ProfilePanel : public juce::Component,
                     public juce::ChangeListener
{
public:
    ProfilePanel(AppState& state, SessionController& controller);
    ~ProfilePanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

    void setTargetServer(const ServerInfo& server);

private:
    void fetchAndPopulate();
    void onLogin();
    void onDisconnect();

    AppState&          appState;
    SessionController& controller;

    ServerInfo         targetServer;
    bool               hasServer = false;

    juce::ListBox  profileListBox;
    SlopInput      pinInput;
    SlopButton     loginBtn;
    SlopButton     disconnectBtn;
    juce::Label    statusLabel;

    struct ProfileListModel;
    std::unique_ptr<ProfileListModel> listModel;
    int connectedProfileId = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProfilePanel)
};