#pragma once
#include <JuceHeader.h>
#include "../App/AppState.h"
#include "../App/SessionController.h"
#include "SlopButton.h"
#include "SlopInput.h"

class ConnectToForm : public juce::Component
{
public:
    std::function<void(const ServerInfo&)> onConnect;
    std::function<void()>                  onCancel;

    ConnectToForm();
    void resized() override;
    void paint(juce::Graphics& g) override;

    void setHost(const juce::String& host);
    void setPort(int port);

    juce::String getHost() const { return ipInput.getText(); }
    int          getPort() const { return portInput.getIntValue(); }

private:
    void fireConnect();

    juce::Label ipLabel, portLabel;
    SlopInput   ipInput, portInput;
    SlopButton  connectBtn, cancelBtn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectToForm)
};

class ConnectToDialog : public juce::DialogWindow
{
public:
    ConnectToDialog(std::function<void(const ServerInfo&)> onConnect);
    void closeButtonPressed() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectToDialog)
};

class ConnectionPanel : public juce::Component,
                        public juce::ChangeListener
{
public:
    std::function<void(const ServerInfo&)> onServerSelected;

    ConnectionPanel(AppState& state, SessionController& controller);
    ~ConnectionPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void changeListenerCallback(juce::ChangeBroadcaster*) override;

private:
    void refreshServerList();
    void onConnectDiscovered(int row);
    void openConnectToDialog();

    AppState&          appState;
    SessionController& controller;

    SlopButton   refreshBtn;
    SlopButton   connectToBtn;
    juce::ListBox serverListBox;

    struct ServerListModel;
    std::unique_ptr<ServerListModel> listModel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionPanel)
};
