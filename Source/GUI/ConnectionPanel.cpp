#include "ConnectionPanel.h"
#include "SlopButton.h"
#include "SlopInput.h"
#include "LucideIcons.h"
#include "SlopLookAndFeel.h"

ConnectToForm::ConnectToForm()
{
    using SL = SlopLookAndFeel;

    ipLabel.setText("HOST ADDRESS", juce::dontSendNotification);
    ipLabel.setFont(juce::Font(SL::FONT_MICRO, juce::Font::bold));
    ipLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    addAndMakeVisible(ipLabel);

    portLabel.setText("PORT", juce::dontSendNotification);
    portLabel.setFont(juce::Font(SL::FONT_MICRO, juce::Font::bold));
    portLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    addAndMakeVisible(portLabel);

    ipInput.setIcon("globe");
    ipInput.setPlaceholder("127.0.0.1");
    ipInput.setText("127.0.0.1");
    ipInput.onReturnKey = [this] { fireConnect(); };
    addAndMakeVisible(ipInput);

    portInput.setIcon("hash");
    portInput.setPlaceholder("3000");
    portInput.setText("3000");
    portInput.setNumericOnly(true, 5);
    portInput.onReturnKey = [this] { fireConnect(); };
    addAndMakeVisible(portInput);

    connectBtn.setStyle(SlopButton::IconAndText);
    connectBtn.setIcon("plug");
    connectBtn.setText("Connect");
    connectBtn.setPrimary(true);
    connectBtn.onClick = [this] { fireConnect(); };
    addAndMakeVisible(connectBtn);

    cancelBtn.setStyle(SlopButton::Text);
    cancelBtn.setText("Cancel");
    cancelBtn.onClick = [this] { if (onCancel) onCancel(); };
    addAndMakeVisible(cancelBtn);

    setSize(360, 230);
}

void ConnectToForm::fireConnect()
{
    ServerInfo s;
    s.serverId    = "manual";
    s.serverName  = ipInput.getText().toStdString();
    s.ip          = ipInput.getText().toStdString();
    s.controlPort = portInput.getIntValue();
    s.pitchPort   = s.controlPort + 1;
    if (onConnect) onConnect(s);
}

void ConnectToForm::setHost(const juce::String& host) { ipInput.setText(host); }
void ConnectToForm::setPort(int port)                 { portInput.setText(juce::String(port)); }

void ConnectToForm::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;

    g.setColour(SL::BG_CARD);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), SL::CORNER_LG);

    const int divY = getHeight() - SL::PAD - SL::BTN_H - SL::PAD_SM;
    g.setColour(SL::BORDER.withAlpha(0.6f));
    g.fillRect(SL::PAD, divY, getWidth() - SL::PAD * 2, 1);
}

void ConnectToForm::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    constexpr int FIELD_H   = 38;
    constexpr int LABEL_H   = 13;
    constexpr int LABEL_GAP = 5;
    constexpr int FIELD_GAP = SlopLookAndFeel::PAD_SM + 4;

    ipLabel.setBounds(area.removeFromTop(LABEL_H));
    area.removeFromTop(LABEL_GAP);
    ipInput.setBounds(area.removeFromTop(FIELD_H));
    area.removeFromTop(FIELD_GAP);

    portLabel.setBounds(area.removeFromTop(LABEL_H));
    area.removeFromTop(LABEL_GAP);
    portInput.setBounds(area.removeFromTop(FIELD_H));

    area.removeFromTop(SL::PAD + 4);

    auto btnRow = area.removeFromTop(SL::BTN_H);
    cancelBtn .setBounds(btnRow.removeFromLeft(90));
    btnRow.removeFromLeft(SL::PAD_SM);
    connectBtn.setBounds(btnRow);
}

ConnectToDialog::ConnectToDialog(std::function<void(const ServerInfo&)> onConnect)
    : juce::DialogWindow("Connect To Server",
                          SlopLookAndFeel::BG_CARD,
                          true, true)
{
    auto* form = new ConnectToForm();

    form->onConnect = [this, cb = std::move(onConnect)](const ServerInfo& s)
    {
        if (cb) cb(s);
        closeButtonPressed();
    };
    form->onCancel = [this] { closeButtonPressed(); };

    setContentOwned(form, true);
    centreAroundComponent(nullptr, getWidth(), getHeight());
    setResizable(false, false);
    setVisible(true);
    enterModalState(true);
}

void ConnectToDialog::closeButtonPressed()
{
    setVisible(false);
    delete this;
}

namespace {

struct ServerRowComponent : public juce::Component
{
    ServerRowComponent() { setInterceptsMouseClicks(false, true); }

    void setOnConnect(std::function<void()> cb) { onConnect = std::move(cb); }

    void paint(juce::Graphics& g) override
    {
        using SL = SlopLookAndFeel;
        const int w = getWidth();
        const int h = getHeight();

        if (isSelected)
        {
            g.setColour(SL::SEL_BG);
            g.fillRoundedRectangle(6.0f, 3.0f, (float)(w - 12), (float)(h - 6), SL::CORNER_SM);

            g.setColour(SL::ACCENT.withAlpha(0.3f));
            g.drawRoundedRectangle(6.5f, 3.5f, (float)(w - 13), (float)(h - 7), SL::CORNER_SM, 1.0f);
        }
        else
        {
            const juce::Colour rowBg = isHovered ? SL::BG_HOVER : SL::BG_ELEVATED.withAlpha(0.6f);
            g.setColour(rowBg);
            g.fillRoundedRectangle(6.0f, 3.0f, (float)(w - 12), (float)(h - 6), SL::CORNER_SM);

            g.setColour(SL::BORDER.withAlpha(isHovered ? 0.7f : 0.4f));
            g.drawRoundedRectangle(6.5f, 3.5f, (float)(w - 13), (float)(h - 7), SL::CORNER_SM, 1.0f);
        }

        if (serverName.isEmpty()) return;

        constexpr float DOT_R = 4.0f;
        constexpr float DOT_X = 22.0f;
        const float dotY = h * 0.5f;

        g.setColour(SL::ACCENT.withAlpha(0.18f));
        g.fillEllipse(DOT_X - DOT_R - 3.0f, dotY - DOT_R - 3.0f,
                      (DOT_R + 3.0f) * 2.0f, (DOT_R + 3.0f) * 2.0f);
        g.setColour(SL::ACCENT);
        g.fillEllipse(DOT_X - DOT_R, dotY - DOT_R, DOT_R * 2.0f, DOT_R * 2.0f);

        g.setColour(isSelected ? SL::TEXT_PRIMARY : SL::TEXT_PRIMARY.withAlpha(0.90f));
        g.setFont(juce::Font(SL::FONT_SUBHEAD, juce::Font::bold));
        g.drawText(serverName, 36, 0, w - 100, h, juce::Justification::centredLeft);

        g.setColour(SL::TEXT_MUTED);
        g.setFont(juce::Font(SL::FONT_CAPTION));
        g.drawText(ipAddress, w / 2 + 8, 0, w / 2 - 56, h,
                   juce::Justification::centredRight);

        const float iconSz = 18.0f;
        const float iconX  = (float)(w - 12) - iconSz;
        const float iconY  = (h - iconSz) * 0.5f;
        const juce::Colour iconCol = isHovered ? SL::ACCENT.brighter(0.2f) : SL::ACCENT.withAlpha(0.7f);
        LucideIcons::draw(g, "arrow-right",
                          { iconX, iconY, iconSz, iconSz },
                          iconCol);
    }

    void mouseEnter(const juce::MouseEvent&) override { isHovered = true; repaint(); }
    void mouseExit(const juce::MouseEvent&)  override { isHovered = false; repaint(); }

    void mouseUp(const juce::MouseEvent& e) override
    {
        if (e.x > getWidth() - 44 && onConnect)
            onConnect();
    }

    void update(int row, bool selected, const ServerInfo& info)
    {
        rowIndex   = row;
        isSelected = selected;
        serverName = juce::String(info.serverName);
        ipAddress  = juce::String(info.ip);
    }

    std::function<void()> onConnect;

private:
    int rowIndex   = 0;
    bool isSelected = false;
    bool isHovered  = false;
    juce::String serverName;
    juce::String ipAddress;
};

}

struct ConnectionPanel::ServerListModel : public juce::ListBoxModel
{
    std::vector<ServerInfo> items;
    std::function<void(int)> onRowActivated;

    int getNumRows() override { return static_cast<int>(items.size()); }

    void paintListBoxItem(int, juce::Graphics&, int, int, bool) override {}

    juce::Component* refreshComponentForRow(int row, bool isSelected,
                                            juce::Component* existing) override
    {
        ServerRowComponent* comp = dynamic_cast<ServerRowComponent*>(existing);
        if (comp == nullptr)
            comp = new ServerRowComponent();

        if (row >= 0 && row < static_cast<int>(items.size()))
        {
            const int idx = row;
            comp->update(row, isSelected, items[static_cast<std::size_t>(row)]);
            comp->setOnConnect([this, idx]() {
                if (onRowActivated) onRowActivated(idx);
            });
        }
        else
        {
            comp->setOnConnect(nullptr);
        }

        return comp;
    }

    void listBoxItemDoubleClicked(int row, const juce::MouseEvent&) override
    {
        if (onRowActivated && row >= 0 && row < static_cast<int>(items.size()))
            onRowActivated(row);
    }
};

ConnectionPanel::ConnectionPanel(AppState& s, SessionController& c)
    : appState(s), controller(c),
      listModel(std::make_unique<ServerListModel>())
{
    using SL = SlopLookAndFeel;

    listModel->onRowActivated = [this](int row) { onConnectDiscovered(row); };

    serverListBox.setModel(listModel.get());
    serverListBox.setRowHeight(46);
    serverListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    serverListBox.setColour(juce::ListBox::outlineColourId,    juce::Colours::transparentBlack);
    serverListBox.setOutlineThickness(0);
    addAndMakeVisible(serverListBox);

    refreshBtn.setStyle(SlopButton::Icon);
    refreshBtn.setIcon("refresh-cw");
    refreshBtn.setSize(30, 30);
    refreshBtn.onClick = [this] {
        controller.startDiscovery();
        refreshBtn.setEnabled(false);
        juce::Timer::callAfterDelay(5000, [this] { refreshBtn.setEnabled(true); });
    };
    addAndMakeVisible(refreshBtn);

    connectToBtn.setStyle(SlopButton::Icon);
    connectToBtn.setIcon("cable");
    connectToBtn.setSize(30, 30);
    connectToBtn.onClick = [this] { openConnectToDialog(); };
    addAndMakeVisible(connectToBtn);

    serverListBox.getViewport()->setScrollBarsShown(true, false);
    appState.addChangeListener(this);
    refreshServerList();
}

ConnectionPanel::~ConnectionPanel()
{
    appState.removeChangeListener(this);
}

void ConnectionPanel::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    auto header = area.removeFromTop(36);

    const bool connected = (appState.getConnectionState() == ConnectionState::Authenticated
                         || appState.getConnectionState() == ConnectionState::Linked
                         || appState.getConnectionState() == ConnectionState::Monitoring);

    const float iconSize = 20.0f;
    const float iconY = (36.0f - iconSize) * 0.5f;
    LucideIcons::draw(g, connected ? "plug" : "network",
                      { (float)area.getX(), iconY + SL::PAD, iconSize, iconSize },
                      SL::ACCENT);

    g.setColour(SL::TEXT_PRIMARY);
    g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
    g.drawText("Discovered Servers",
               area.getX() + (int)iconSize + 8, SL::PAD,
               area.getWidth() - (int)iconSize - 80, 36,
               juce::Justification::centredLeft);

    area.removeFromTop(SL::PAD_SM);
    g.setColour(SL::BORDER.withAlpha(0.8f));
    g.fillRect(area.getX(), area.getY(), area.getWidth(), 1);

    if (listModel->items.empty())
    {
        const int listTop = area.getY() + SL::PAD_SM + 1;
        const int listH   = getHeight() - listTop - SL::PAD;

        g.setColour(SL::TEXT_MUTED.withAlpha(0.4f));
        const float emptyIconSz = 40.0f;
        const float emptyIconX = (getWidth() - SL::PAD * 2 - emptyIconSz) * 0.5f + SL::PAD;
        const float emptyIconY = listTop + (listH - emptyIconSz - 28) * 0.5f;
        LucideIcons::draw(g, "wifi-off",
                          { emptyIconX, emptyIconY, emptyIconSz, emptyIconSz },
                          SL::TEXT_MUTED.withAlpha(0.3f));

        g.setFont(juce::Font(SL::FONT_BODY));
        g.drawText("No servers found",
                   0, (int)(emptyIconY + emptyIconSz + 10),
                   getWidth(), 20, juce::Justification::centred);

        g.setColour(SL::TEXT_MUTED.withAlpha(0.35f));
        g.setFont(juce::Font(SL::FONT_CAPTION));
        g.drawText("Use refresh or connect manually",
                   0, (int)(emptyIconY + emptyIconSz + 32),
                   getWidth(), 16, juce::Justification::centred);
    }
}

void ConnectionPanel::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    auto header = area.removeFromTop(36);
    connectToBtn.setBounds(header.removeFromRight(30).withSizeKeepingCentre(30, 30));
    header.removeFromRight(SL::PAD_XS);
    refreshBtn.setBounds(header.removeFromRight(30).withSizeKeepingCentre(30, 30));

    area.removeFromTop(SL::PAD_SM + 1);
    serverListBox.setBounds(area);
}

void ConnectionPanel::changeListenerCallback(juce::ChangeBroadcaster*)
{
    refreshServerList();
}

void ConnectionPanel::refreshServerList()
{
    listModel->items = appState.getServers();
    serverListBox.updateContent();
    serverListBox.repaint();
    repaint();
}

void ConnectionPanel::onConnectDiscovered(int row)
{
    if (row < 0 || static_cast<std::size_t>(row) >= listModel->items.size())
        return;
    if (onServerSelected)
        onServerSelected(listModel->items[static_cast<std::size_t>(row)]);
}

void ConnectionPanel::openConnectToDialog()
{
    auto* dlg = new ConnectToDialog([this](const ServerInfo& server) {
        if (onServerSelected)
            onServerSelected(server);
    });
    juce::ignoreUnused(dlg);
}
