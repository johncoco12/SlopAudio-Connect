#include "MainWindow.h"

MainContent::MainContent(AppState& s, SessionController& c)
    : appState(s), controller(c),
      sidebar(s),
      connectionPanel(s, c),
      profilePanel(s, c),
      audioPanel(c.getAudioEngine()),
      pluginPanel(c.getAudioEngine().getPluginChain()),
      detectionPanel(c.getAudioEngine()),
      statusBar(s)
{
    setLookAndFeel(&laf);

    addAndMakeVisible(sidebar);
    addAndMakeVisible(connectionPanel);
    addAndMakeVisible(profilePanel);
    addAndMakeVisible(audioPanel);
    addAndMakeVisible(pluginPanel);
    addAndMakeVisible(detectionPanel);
    addAndMakeVisible(statusBar);

    sidebar.onPageChanged = [this](int idx) { setPage(idx); };

    connectionPanel.onServerSelected = [this](const ServerInfo& server)
    {
        profilePanel.setTargetServer(server);
        setPage(1);
    };

    setPage(0);
    setSize(720, 580);
}

MainContent::~MainContent()
{
    setLookAndFeel(nullptr);
}

void MainContent::setPage(int idx)
{
    currentPage = idx;
    connectionPanel.setVisible(idx == 0);
    profilePanel   .setVisible(idx == 1);
    pluginPanel    .setVisible(idx == 2);
    audioPanel     .setVisible(idx == 3);
    detectionPanel .setVisible(idx == 4);
    sidebar.setCurrentPage(idx);
}

void MainContent::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;

    g.setColour(SL::BG_DEEP);
    g.fillRect(0, 0, SL::SIDEBAR_W, getHeight());

    g.setColour(SL::BG_CARD);
    g.fillRect(SL::SIDEBAR_W, 0,
               getWidth() - SL::SIDEBAR_W,
               getHeight() - SL::STATUS_H);

    juce::ColourGradient topShadow(juce::Colour(0x18000000), 0.0f, (float)0,
                                    juce::Colours::transparentBlack, 0.0f, 8.0f, false);
    g.setGradientFill(topShadow);
    g.fillRect(SL::SIDEBAR_W, 0, getWidth() - SL::SIDEBAR_W, 8);
}

void MainContent::resized()
{
    using SL = SlopLookAndFeel;
    sidebar   .setBounds(0, 0, SL::SIDEBAR_W, getHeight() - SL::STATUS_H);
    statusBar .setBounds(0, getHeight() - SL::STATUS_H, getWidth(), SL::STATUS_H);

    const juce::Rectangle<int> content {
        SL::SIDEBAR_W, 0,
        getWidth() - SL::SIDEBAR_W,
        getHeight() - SL::STATUS_H
    };

    connectionPanel.setBounds(content);
    profilePanel   .setBounds(content);
    pluginPanel    .setBounds(content);
    audioPanel     .setBounds(content);
    detectionPanel .setBounds(content);
}

MainWindow::MainWindow(const juce::String& name,
                       AppState& state,
                       SessionController& controller)
    : juce::DocumentWindow(name,
                           SlopLookAndFeel::BG_DEEP,
                           DocumentWindow::allButtons)
{
    setUsingNativeTitleBar(false);
    setContentOwned(new MainContent(state, controller), true);
    setResizable(true, true);
    centreWithSize(720, 580);
    setVisible(true);
}

void MainWindow::closeButtonPressed()
{
    juce::JUCEApplication::getInstance()->systemRequestedQuit();
}
