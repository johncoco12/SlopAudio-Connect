#include "ProfilePanel.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"
#include "../App/ProfileManager.h"

namespace {
    const juce::Colour PROFILE_COLORS[] = {
        juce::Colour(0xff4080e0), juce::Colour(0xffe8c040),
        juce::Colour(0xffef4444), juce::Colour(0xff22c55e),
        juce::Colour(0xffa855f7), juce::Colour(0xfff97316),
        juce::Colour(0xff06b6d4), juce::Colour(0xffec4899),
        juce::Colour(0xff84cc16), juce::Colour(0xff6366f1),
    };
    static constexpr int NUM_COLORS = 10;

    juce::Colour profileColour(int id)
    {
        return PROFILE_COLORS[((id % NUM_COLORS) + NUM_COLORS) % NUM_COLORS];
    }

    bool isConnectedState(ConnectionState s)
    {
        return s == ConnectionState::Authenticated
            || s == ConnectionState::Linked
            || s == ConnectionState::Monitoring;
    }
}

struct ProfilePanel::ProfileListModel : public juce::ListBoxModel
{
    std::vector<ProfileInfo> items;
    int connectedProfileId = -1;

    int getNumRows() override { return static_cast<int>(items.size()); }

    void paintListBoxItem(int row, juce::Graphics& g, int w, int h, bool selected) override
    {
        using SL = SlopLookAndFeel;

        if (row >= static_cast<int>(items.size())) return;
        const auto& p   = items[static_cast<std::size_t>(row)];
        const juce::Colour col = profileColour(p.id);
        const bool isConnected = (p.id == connectedProfileId);

        const float padH = 5.0f;
        const float padV = 4.0f;
        const float r    = SL::CORNER_SM;
        const float rowX = padH;
        const float rowY = padV;
        const float rowW = (float)w - padH * 2.0f;
        const float rowH = (float)h - padV * 2.0f;

        if (isConnected)
        {
            g.setColour(SL::SUCCESS.withAlpha(0.08f));
            g.fillRoundedRectangle(rowX, rowY, rowW, rowH, r);
            g.setColour(SL::SUCCESS.withAlpha(0.35f));
            g.drawRoundedRectangle(rowX + 0.5f, rowY + 0.5f, rowW - 1.0f, rowH - 1.0f, r, 1.0f);
        }
        else if (selected)
        {
            g.setColour(SL::SEL_BG);
            g.fillRoundedRectangle(rowX, rowY, rowW, rowH, r);
            g.setColour(col.withAlpha(0.45f));
            g.drawRoundedRectangle(rowX + 0.5f, rowY + 0.5f, rowW - 1.0f, rowH - 1.0f, r, 1.0f);
        }
        else
        {
            g.setColour(SL::BG_ELEVATED);
            g.fillRoundedRectangle(rowX, rowY, rowW, rowH, r);
            g.setColour(SL::BORDER.withAlpha(0.6f));
            g.drawRoundedRectangle(rowX + 0.5f, rowY + 0.5f, rowW - 1.0f, rowH - 1.0f, r, 1.0f);
        }

        constexpr float AV_R = 16.0f;
        const float avX = rowX + 14.0f + AV_R;
        const float avY = h * 0.5f;

        const juce::Colour avatarCol = isConnected ? SL::SUCCESS : col;

        g.setColour(avatarCol.withAlpha(isConnected || selected ? 0.35f : 0.18f));
        g.fillEllipse(avX - AV_R, avY - AV_R, AV_R * 2.0f, AV_R * 2.0f);

        g.setColour(avatarCol.withAlpha(isConnected || selected ? 0.7f : 0.4f));
        g.drawEllipse(avX - AV_R + 0.5f, avY - AV_R + 0.5f,
                      AV_R * 2.0f - 1.0f, AV_R * 2.0f - 1.0f, 1.5f);

        g.setColour(avatarCol);
        g.setFont(juce::Font(SL::FONT_LABEL, juce::Font::bold));
        const juce::String initial = p.name.isNotEmpty()
            ? p.name.substring(0, 1).toUpperCase() : "?";
        g.drawText(initial,
                   (int)(avX - AV_R), (int)(avY - AV_R),
                   (int)(AV_R * 2.0f), (int)(AV_R * 2.0f),
                   juce::Justification::centred);

        g.setColour(isConnected ? SL::TEXT_PRIMARY : (selected ? SL::TEXT_PRIMARY : SL::TEXT_PRIMARY.withAlpha(0.85f)));
        g.setFont(juce::Font(SL::FONT_SUBHEAD, juce::Font::bold));
        g.drawText(p.name, (int)(avX + AV_R + 10.0f), 0, w - (int)(avX + AV_R + 20.0f), h,
                   juce::Justification::centredLeft);

        if (isConnected)
        {

            const juce::Font pillFont(SL::FONT_MICRO, juce::Font::bold);
            const juce::String pillText = "Connected";
            const float pillW = pillFont.getStringWidthFloat(pillText) + 14.0f;
            const float pillH = 17.0f;
            const float pillX = rowX + rowW - pillW - 8.0f;
            const float pillY = (h - pillH) * 0.5f;

            g.setColour(SL::SUCCESS.withAlpha(0.15f));
            g.fillRoundedRectangle(pillX, pillY, pillW, pillH, pillH * 0.5f);
            g.setColour(SL::SUCCESS.withAlpha(0.5f));
            g.drawRoundedRectangle(pillX + 0.5f, pillY + 0.5f, pillW - 1.0f, pillH - 1.0f,
                                   (pillH - 1.0f) * 0.5f, 1.0f);
            g.setFont(pillFont);
            g.setColour(SL::SUCCESS);
            g.drawText(pillText, (int)pillX, (int)pillY, (int)pillW, (int)pillH,
                       juce::Justification::centred);
        }
        else if (selected)
        {
            const float iconSz = 15.0f;
            const float iconX  = rowX + rowW - 10.0f - iconSz;
            const float iconY2 = (h - iconSz) * 0.5f;
            LucideIcons::draw(g, "circle-check", { iconX, iconY2, iconSz, iconSz }, col);
        }
    }
};

ProfilePanel::ProfilePanel(AppState& s, SessionController& c)
    : appState(s), controller(c),
      listModel(std::make_unique<ProfileListModel>())
{
    using SL = SlopLookAndFeel;

    profileListBox.setModel(listModel.get());
    profileListBox.setRowHeight(50);
    profileListBox.setColour(juce::ListBox::backgroundColourId, juce::Colours::transparentBlack);
    profileListBox.setColour(juce::ListBox::outlineColourId, juce::Colours::transparentBlack);
    profileListBox.setOutlineThickness(0);
    addAndMakeVisible(profileListBox);

    pinInput.setIcon("lock");
    pinInput.setPlaceholder("Enter PIN");
    pinInput.setNumericOnly(true, 8);
    pinInput.setPasswordMode(true);
    pinInput.onReturnKey = [this] { onLogin(); };
    addAndMakeVisible(pinInput);

    loginBtn.setStyle(SlopButton::IconAndText);
    loginBtn.setIcon("log-in");
    loginBtn.setText("Login");
    loginBtn.setPrimary(true);
    loginBtn.onClick = [this] { onLogin(); };
    addAndMakeVisible(loginBtn);

    disconnectBtn.setStyle(SlopButton::IconAndText);
    disconnectBtn.setIcon("log-out");
    disconnectBtn.setText("Disconnect");
    disconnectBtn.onClick = [this] { onDisconnect(); };
    addAndMakeVisible(disconnectBtn);

    statusLabel.setJustificationType(juce::Justification::centred);
    statusLabel.setFont(juce::Font(SL::FONT_LABEL));
    addAndMakeVisible(statusLabel);

    appState.addChangeListener(this);
}

ProfilePanel::~ProfilePanel()
{
    appState.removeChangeListener(this);
}

void ProfilePanel::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    const int W = getWidth();
    const int H = getHeight();

    g.fillAll(SL::BG_CARD);

    const bool connected = isConnectedState(appState.getConnectionState());

    {
        auto area   = getLocalBounds().reduced(SL::PAD);
        auto header = area.removeFromTop(36);

        const float iconSize = 20.0f;
        const float iconY    = (36.0f - iconSize) * 0.5f;
        LucideIcons::draw(g, "circle-user-round",
                          { (float)area.getX(), iconY + SL::PAD, iconSize, iconSize },
                          SL::ACCENT);

        g.setColour(SL::TEXT_PRIMARY);
        g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
        g.drawText("Select Profile",
                   area.getX() + (int)iconSize + 8, SL::PAD,
                   area.getWidth() - (int)iconSize - 8, 36,
                   juce::Justification::centredLeft);

        area.removeFromTop(SL::PAD_SM);

        g.setColour(SL::BORDER.withAlpha(0.8f));
        g.fillRect(area.getX(), area.getY(), area.getWidth(), 1);

        if (hasServer)
        {
            area.removeFromTop(1);
            juce::String serverLine = juce::String(targetServer.serverName)
                                     + "  ·  " + juce::String(targetServer.ip);

            const juce::Font captionFont(SL::FONT_CAPTION);
            const float pillW = captionFont.getStringWidthFloat(serverLine) + 20.0f;
            const float pillH = 18.0f;
            const float pillX = area.getX();
            const float pillY = (float)area.getY() + SL::PAD_SM;

            g.setColour(SL::BG_ELEVATED);
            g.fillRoundedRectangle(pillX, pillY, pillW, pillH, pillH * 0.5f);
            g.setColour(SL::BORDER);
            g.drawRoundedRectangle(pillX + 0.5f, pillY + 0.5f, pillW - 1.0f, pillH - 1.0f,
                                   (pillH - 1.0f) * 0.5f, 1.0f);

            g.setColour(SL::TEXT_MUTED);
            g.setFont(captionFont);
            g.drawText(serverLine, (int)pillX, (int)pillY, (int)pillW, (int)pillH,
                       juce::Justification::centred);
        }
    }

    juce::ignoreUnused(H);
}

void ProfilePanel::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    area.removeFromTop(36 + SL::PAD_SM + 1);

    if (hasServer)
        area.removeFromTop(SL::PAD_SM + 18 + SL::PAD_SM);

    area.removeFromTop(SL::PAD_SM);

    const bool connected = isConnectedState(appState.getConnectionState());

    if (connected)
    {

        const int formH = SL::BTN_H + 8;
        const int listH = area.getHeight() - formH - SL::SECTION_GAP - SL::PAD_SM - 24;

        profileListBox.setBounds(area.removeFromTop(juce::jmax(60, listH)));
        area.removeFromTop(SL::SECTION_GAP);

        disconnectBtn.setBounds(area.removeFromTop(formH));
        pinInput    .setBounds({});
        loginBtn    .setBounds({});
    }
    else
    {

        const int formH = SL::BTN_H + 8;
        const int listH = area.getHeight() - formH - SL::SECTION_GAP - SL::PAD_SM - 24;

        profileListBox.setBounds(area.removeFromTop(juce::jmax(60, listH)));
        area.removeFromTop(SL::SECTION_GAP);

        auto pinRow = area.removeFromTop(formH);
        loginBtn.setBounds(pinRow.removeFromRight(90));
        pinRow.removeFromRight(SL::PAD_SM);
        pinInput.setBounds(pinRow);
        disconnectBtn.setBounds({});
    }

    area.removeFromTop(SL::PAD_SM);
    statusLabel.setBounds(area.removeFromBottom(24));
}

void ProfilePanel::changeListenerCallback(juce::ChangeBroadcaster*)
{
    using SL = SlopLookAndFeel;
    const ConnectionState cs = appState.getConnectionState();

    profileListBox.setEnabled(!isConnectedState(cs));
    pinInput.setEnabled(!isConnectedState(cs));
    loginBtn.setEnabled(!isConnectedState(cs));

    if (isConnectedState(cs))
    {
        const auto* profile = appState.getActiveProfile();
        connectedProfileId = profile ? profile->id : -1;
    }
    else
    {
        connectedProfileId = -1;
    }
    listModel->connectedProfileId = connectedProfileId;

    if (isConnectedState(cs))
    {
        const auto* profile = appState.getActiveProfile();
        const juce::String name = profile ? profile->name : "Profile";
        statusLabel.setColour(juce::Label::textColourId, SL::SUCCESS);
        statusLabel.setText("Connected as " + name, juce::dontSendNotification);
    }
    else if (cs == ConnectionState::Connecting)
    {
        statusLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
        statusLabel.setText("Connecting...", juce::dontSendNotification);
    }
    else
    {
        statusLabel.setColour(juce::Label::textColourId, SL::WARNING);
        const juce::String err = appState.getLastError();
        statusLabel.setText(err.isNotEmpty() ? err : "Select a profile and enter PIN",
                           juce::dontSendNotification);
    }

    listModel->items = appState.getProfiles();
    profileListBox.updateContent();
    profileListBox.repaint();
    resized();
}

void ProfilePanel::setTargetServer(const ServerInfo& server)
{
    targetServer = server;
    hasServer    = true;
    fetchAndPopulate();
    resized();
}

void ProfilePanel::fetchAndPopulate()
{
    using SL = SlopLookAndFeel;
    statusLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    statusLabel.setText("Fetching profiles...", juce::dontSendNotification);

    ProfileManager pm;
    pm.fetchProfiles(
        targetServer.ip, targetServer.httpPort,
        [this](std::vector<ProfileInfo> profiles)
        {
            appState.setProfiles(std::move(profiles));
            const juce::String hint = listModel->items.empty()
                ? "No profiles found" : "Select a profile and enter PIN";
            statusLabel.setText(hint, juce::dontSendNotification);
        },
        [this](juce::String err) { appState.setLastError(err); });
}

void ProfilePanel::onLogin()
{
    if (!hasServer) return;

    const int row = profileListBox.getSelectedRow();
    const auto& profiles = appState.getProfiles();
    if (row < 0 || row >= static_cast<int>(profiles.size())) return;

    const juce::String pin = pinInput.getText();
    if (pin.isEmpty()) return;

    const auto idx = static_cast<std::size_t>(row);
    appState.setActiveProfile(profiles[idx]);
    controller.connectToServer(targetServer, profiles[idx].id, pin);
}

void ProfilePanel::onDisconnect()
{
    controller.disconnect();
    pinInput.setText("", false);
}
