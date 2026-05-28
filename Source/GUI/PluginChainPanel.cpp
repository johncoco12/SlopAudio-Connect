#include "PluginChainPanel.h"
#include "SlopLookAndFeel.h"
#include "LucideIcons.h"

namespace {

struct PluginRowComponent : public juce::Component
{
    PluginRowComponent() { setInterceptsMouseClicks(false, false); }

    void update(int row, bool selected, juce::AudioPluginInstance* p,
                bool bypassed, bool hovered, bool bpHovered,
                bool deleteHovered, bool ghost)
    {
        rowIndex       = row;
        isSelected     = selected;
        plugin         = p;
        isBypassed     = bypassed;
        isHovered      = hovered;
        bypassHovered  = bpHovered;
        delHovered     = deleteHovered;
        isDragGhost    = ghost;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        using SL = SlopLookAndFeel;
        const int w = getWidth(), h = getHeight();
        const float padH = 5.0f, padV = 3.0f;
        const float rw = w - padH*2, rh = h - padV*2;

        if (isDragGhost)
        {
            g.setColour(SL::ACCENT.withAlpha(0.12f));
            g.fillRoundedRectangle(padH, padV, rw, rh, SL::CORNER_SM);
            g.setColour(SL::ACCENT.withAlpha(0.4f));
            g.drawRoundedRectangle(padH+.5f, padV+.5f, rw-1, rh-1, SL::CORNER_SM, 1.2f);
            return;
        }

        if (isBypassed)
        {
            g.setColour(SL::BG_DEEP);
            g.fillRoundedRectangle(padH, padV, rw, rh, SL::CORNER_SM);
            g.setColour(SL::BORDER.withAlpha(0.4f));
        }
        else if (isSelected)
        {
            g.setColour(SL::SEL_BG);
            g.fillRoundedRectangle(padH, padV, rw, rh, SL::CORNER_SM);
            g.setColour(SL::ACCENT.withAlpha(0.4f));
        }
        else
        {
            g.setColour(isHovered ? SL::BG_HOVER : SL::BG_ELEVATED);
            g.fillRoundedRectangle(padH, padV, rw, rh, SL::CORNER_SM);
            g.setColour(SL::BORDER.withAlpha(isHovered ? 0.7f : 0.5f));
        }
        g.drawRoundedRectangle(padH+.5f, padV+.5f, rw-1, rh-1, SL::CORNER_SM, 1.0f);

        if (!plugin) return;

        LucideIcons::draw(g, "grip-vertical",
                          { padH+4, (float)(h-14)*.5f, 12, 14 },
                          SL::TEXT_MUTED.withAlpha(isHovered ? 0.5f : 0.22f));

        const float bx = padH+20, by = (float)(h-17)*.5f;
        g.setColour(isBypassed ? SL::BG_ELEVATED : SL::ACCENT_DIM.brighter(0.5f));
        g.fillRoundedRectangle(bx, by, 22, 17, SL::CORNER_SM-1);
        g.setColour(isBypassed ? SL::TEXT_MUTED : SL::ACCENT);
        g.setFont(juce::Font(SL::FONT_CAPTION, juce::Font::bold));
        g.drawText(juce::String(rowIndex+1), (int)bx,(int)by,22,17,
                   juce::Justification::centred);

        const float iconX = bx+30, iconY = (float)(h-15)*.5f;
        LucideIcons::draw(g, "puzzle", {iconX,iconY,15,15},
                          isBypassed ? SL::TEXT_MUTED.withAlpha(0.4f)
                          : (isSelected ? SL::ACCENT : SL::TEXT_MUTED));

        const int nameX = (int)(iconX+23);
        const int nameW = w - nameX - 96;
        g.setColour(isBypassed ? SL::TEXT_MUTED.withAlpha(0.45f)
                    : (isSelected ? SL::TEXT_PRIMARY : SL::TEXT_PRIMARY.withAlpha(0.88f)));
        g.setFont(juce::Font(SL::FONT_SUBHEAD, isBypassed ? juce::Font::plain : juce::Font::bold));
        g.drawText(plugin->getName(), nameX, 0, nameW, h,
                   juce::Justification::centredLeft);

        if (isBypassed)
        {
            g.setColour(SL::WARNING.withAlpha(0.6f));
            g.setFont(juce::Font(SL::FONT_MICRO, juce::Font::bold));
            g.drawText("BYPASSED", nameX + nameW - 4, 0, 56, h,
                       juce::Justification::centredLeft);
        }

        const float editX = (float)(w - 10) - 15;
        const float editY = (float)(h-15)*.5f;
        LucideIcons::draw(g, "sliders-horizontal", {editX, editY, 15, 15},
                          isHovered ? SL::ACCENT.brighter(0.15f)
                                    : SL::ACCENT.withAlpha(isBypassed ? 0.3f : 0.6f));

        const float bpX = editX - 6 - 36, bpY = (float)(h-17)*.5f;
        if (isBypassed)
        {
            g.setColour(SL::BG_ELEVATED);
            g.fillRoundedRectangle(bpX, bpY, 36, 17, 8.5f);
            g.setColour(SL::BORDER_LIGHT);
            g.drawRoundedRectangle(bpX+.5f, bpY+.5f, 35, 16, 8.0f, 1.0f);
            g.setColour(SL::TEXT_MUTED.withAlpha(0.6f));
            g.fillEllipse(bpX+4, bpY+3, 11, 11);
        }
        else
        {
            g.setColour(SL::ACCENT.withAlpha(bypassHovered ? 0.9f : 0.75f));
            g.fillRoundedRectangle(bpX, bpY, 36, 17, 8.5f);
            g.setColour(juce::Colours::white.withAlpha(0.95f));
            g.fillEllipse(bpX+21, bpY+3, 11, 11);
        }

        const float delX = bpX - 6 - 15, delY = (float)(h-15)*.5f;
        const juce::Colour delCol = delHovered
            ? SL::DANGER.brighter(0.1f)
            : SL::DANGER.withAlpha(isHovered ? 0.65f : 0.30f);
        LucideIcons::draw(g, "trash-2", {delX, delY, 15, 15}, delCol);

        editRect   = { (int)(editX-5), 0, 25, h };
        bypassRect = { (int)bpX, (int)bpY, 36, 17 };
        deleteRect = { (int)(delX-5), 0, 25, h };
    }

    juce::AudioPluginInstance* plugin      = nullptr;
    int  rowIndex     = 0;
    bool isSelected   = false, isHovered   = false;
    bool bypassHovered= false, isBypassed  = false;
    bool delHovered   = false;
    bool isDragGhost  = false;

    mutable juce::Rectangle<int> bypassRect, editRect, deleteRect;
};

}

struct PluginChainPanel::DragList : public juce::Component,
                                    public juce::ScrollBar::Listener
{
    static constexpr int ROW_H       = 44;
    static constexpr int DRAG_THRESH = 6;

    PluginChainProcessor*          chain   = nullptr;
    std::function<void(int)>       onEdit;
    std::function<void(int,bool)>  onBypass;
    std::function<void(int,int)>   onMove;
    std::function<void(int)>       onDelete;

    int selectedRow = -1;

    int  dragRow      = -1, dropTarget  = -1;
    int  dragStartY   = 0,  dragCurrentY = 0;
    bool dragging     = false;

    int  hoverRow = -1;
    bool bpHover  = false, delHover = false;

    juce::ScrollBar scrollBar { true };
    int scrollOffset = 0;

    DragList()  { scrollBar.addListener(this); addAndMakeVisible(scrollBar); }
    ~DragList() override { scrollBar.removeListener(this); }

    void scrollBarMoved(juce::ScrollBar*, double pos) override
    { scrollOffset = (int)pos; layoutRows(); repaint(); }

    int rowCount() const { return chain ? chain->getPluginCount() : 0; }

    void refresh()
    {
        const int n = rowCount();
        while ((int)rows.size() < n) { auto* r = new PluginRowComponent(); rows.emplace_back(r); addAndMakeVisible(r); }
        while ((int)rows.size() > n) { removeChildComponent(rows.back().get()); rows.pop_back(); }
        layoutRows();
        updateScrollBar();
        repaintRows();
    }

    void repaintRows()
    {
        for (int i = 0; i < rowCount(); ++i)
        {
            rows[(size_t)i]->update(i,
                i == selectedRow,
                chain->getPlugin(i),
                chain->isBypassed(i),
                i == hoverRow && !dragging,
                i == hoverRow && bpHover  && !dragging,
                i == hoverRow && delHover && !dragging,
                dragging && i == dragRow);
        }
        repaint();
    }

    void layoutRows()
    {
        const int sbW = needsScrollBar() ? 8 : 0;
        const int lw  = getWidth() - sbW;
        for (int i = 0; i < (int)rows.size(); ++i)
            rows[(size_t)i]->setBounds(0, i*ROW_H - scrollOffset, lw, ROW_H);
    }

    bool needsScrollBar() const { return rowCount()*ROW_H > getHeight(); }

    void updateScrollBar()
    {
        const int th = rowCount()*ROW_H;
        if (th > getHeight())
        {
            scrollBar.setVisible(true);
            scrollBar.setRangeLimits(0, th);
            scrollBar.setCurrentRange(scrollOffset, getHeight());
            scrollBar.setBounds(getWidth()-8, 0, 8, getHeight());
        }
        else { scrollBar.setVisible(false); scrollOffset = 0; }
    }

    void resized() override { layoutRows(); updateScrollBar(); }

    int rowAt(int y) const
    {
        const int i = (y + scrollOffset) / ROW_H;
        return (i >= 0 && i < rowCount()) ? i : -1;
    }

    int dropIndexAt(int y) const
    {
        return juce::jlimit(0, rowCount(), (y + scrollOffset + ROW_H/2) / ROW_H);
    }

    void paint(juce::Graphics& g) override
    {
        using SL = SlopLookAndFeel;
        if (!dragging || dropTarget < 0) return;
        const int sbW = needsScrollBar() ? 8 : 0;
        const float lw = (float)(getWidth() - sbW);

        const int gy = dragCurrentY - ROW_H/2;
        g.setColour(SL::BG_ELEVATED.withAlpha(0.94f));
        g.fillRoundedRectangle(5, (float)gy+3, lw-10, (float)ROW_H-6, SL::CORNER_SM);
        g.setColour(SL::ACCENT.withAlpha(0.75f));
        g.drawRoundedRectangle(5.5f, (float)gy+3.5f, lw-11, (float)ROW_H-7, SL::CORNER_SM, 1.5f);
        if (chain && dragRow >= 0 && dragRow < rowCount())
        {
            LucideIcons::draw(g, "grip-vertical", {7.0f,(float)gy+(ROW_H-14)*.5f,12,14},
                              SL::ACCENT.withAlpha(0.6f));
            g.setColour(SL::TEXT_PRIMARY.withAlpha(0.8f));
            g.setFont(juce::Font(SL::FONT_SUBHEAD, juce::Font::bold));
            g.drawText(chain->getPlugin(dragRow)->getName(),
                       22, gy, (int)lw-44, ROW_H, juce::Justification::centredLeft);
        }

        const float ly = (float)(dropTarget*ROW_H - scrollOffset);
        g.setColour(SL::ACCENT);
        g.fillRoundedRectangle(6, ly-1.5f, lw-12, 3, 1.5f);
        g.fillEllipse(2, ly-4, 8, 8);
        g.fillEllipse(lw-10, ly-4, 8, 8);
    }

    void mouseMove(const juce::MouseEvent& e) override
    {
        const int row = rowAt(e.y);
        bool newBp = false, newDel = false;
        if (row >= 0 && row < (int)rows.size())
        {
            auto* r = rows[(size_t)row].get();
            const auto p = r->getLocalPoint(this, e.position).toInt();
            newBp  = r->bypassRect.contains(p);
            newDel = r->deleteRect.contains(p);
        }
        if (row != hoverRow || newBp != bpHover || newDel != delHover)
        {
            hoverRow = row; bpHover = newBp; delHover = newDel;
            repaintRows();
        }
    }

    void mouseEnter(const juce::MouseEvent& e) override { mouseMove(e); }
    void mouseExit (const juce::MouseEvent&) override
    { hoverRow = -1; bpHover = false; delHover = false; repaintRows(); }

    void mouseDown(const juce::MouseEvent& e) override
    {
        const int row = rowAt(e.y);
        if (row < 0) return;
        selectedRow = row; dragRow = row;
        dragStartY = dragCurrentY = e.y;
        dragging = false;
        repaintRows();
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        if (dragRow < 0) return;
        if (!dragging && std::abs(e.y - dragStartY) > DRAG_THRESH)
            dragging = true;
        if (dragging)
        {
            dragCurrentY = juce::jlimit(0, getHeight()-1, e.y);
            dropTarget   = dropIndexAt(dragCurrentY);
            constexpr int EDGE = 30, SPD = 5;
            if (e.y < EDGE && scrollOffset > 0)
            { scrollOffset = std::max(0, scrollOffset-SPD); updateScrollBar(); layoutRows(); }
            else if (e.y > getHeight()-EDGE)
            { const int ms = std::max(0,rowCount()*ROW_H-getHeight()); scrollOffset=std::min(ms,scrollOffset+SPD); updateScrollBar(); layoutRows(); }
            repaintRows();
        }
    }

    void mouseUp(const juce::MouseEvent& e) override
    {
        if (dragging)
        {
            int to = dropTarget;
            if (to > dragRow) to--;
            if (to != dragRow && to >= 0 && to < rowCount() && onMove)
                onMove(dragRow, to);
        }
        else if (dragRow >= 0 && dragRow < (int)rows.size())
        {
            auto* r   = rows[(size_t)dragRow].get();
            const auto pos = r->getLocalPoint(this, e.position).toInt();
            if (r->deleteRect.contains(pos))
            { if (onDelete) onDelete(dragRow); }
            else if (r->bypassRect.contains(pos))
            { if (onBypass) onBypass(dragRow, !chain->isBypassed(dragRow)); }
            else if (r->editRect.contains(pos))
            { if (onEdit) onEdit(dragRow); }
        }
        dragging = false; dragRow = -1; dropTarget = -1;
        repaintRows();
    }

    void mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& w) override
    {
        if (!needsScrollBar()) return;
        const int ms = std::max(0, rowCount()*ROW_H - getHeight());
        scrollOffset = juce::jlimit(0, ms, scrollOffset - (int)(w.deltaY*60.0f));
        updateScrollBar(); layoutRows(); repaint();
    }

private:
    std::vector<std::unique_ptr<PluginRowComponent>> rows;
};

PluginChainPanel::~PluginChainPanel() = default;

PluginChainPanel::PluginChainPanel(PluginChainProcessor& c)
    : chain(c), dragList(std::make_unique<DragList>())
{
    using SL = SlopLookAndFeel;

    dragList->chain = &chain;

    dragList->onEdit   = [this](int idx) { openEditorFor(idx); };
    dragList->onBypass = [this](int idx, bool b) { chain.setBypassed(idx,b); dragList->repaintRows(); repaint(); };
    dragList->onMove   = [this](int from, int to) { chain.movePlugin(from,to); dragList->selectedRow=to; dragList->refresh(); repaint(); };
    dragList->onDelete = [this](int idx) { onRemovePlugin(idx); };

    addAndMakeVisible(*dragList);

    addButton.setStyle(SlopButton::Icon);
    addButton.setIcon("plus");
    addButton.onClick = [this] { onAddPlugin(); };
    addAndMakeVisible(addButton);

    scanButton.setStyle(SlopButton::Icon);
    scanButton.setIcon("scan-search");
    scanButton.onClick = [this] { onScanPlugins(); };
    addAndMakeVisible(scanButton);

    scanStatus.setJustificationType(juce::Justification::centredLeft);
    scanStatus.setFont(juce::Font(SL::FONT_CAPTION));
    scanStatus.setColour(juce::Label::textColourId, SL::TEXT_MUTED);
    addAndMakeVisible(scanStatus);

    hintLabel.setText("Drag to reorder  |  trash to delete  |  slider icon to edit",
                      juce::dontSendNotification);
    hintLabel.setFont(juce::Font(SL::FONT_CAPTION));
    hintLabel.setColour(juce::Label::textColourId, SL::TEXT_MUTED.withAlpha(0.5f));
    hintLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(hintLabel);

    chain.onChainChanged = [this] { dragList->refresh(); repaint(); };
    dragList->refresh();
}

void PluginChainPanel::openEditorFor(int index)
{
    auto* plugin = chain.getPlugin(index);
    if (!plugin) return;
    if (openEditors.count(index) && openEditors[index]) { openEditors[index]->toFront(true); return; }
    openEditors[index] = new PluginEditorWindow(plugin, [this,index](){ openEditors.erase(index); });
}

void PluginChainPanel::paint(juce::Graphics& g)
{
    using SL = SlopLookAndFeel;
    g.fillAll(SL::BG_CARD);

    auto area = getLocalBounds().reduced(SL::PAD);

    const float iconSize = 20.0f;
    LucideIcons::draw(g, "puzzle",
                      { (float)area.getX(), (float)SL::PAD+(36.0f-iconSize)*.5f, iconSize, iconSize },
                      SL::ACCENT);

    g.setColour(SL::TEXT_PRIMARY);
    g.setFont(juce::Font(SL::FONT_HEADING, juce::Font::bold));
    g.drawText("Plugin Chain",
               area.getX()+(int)iconSize+8, SL::PAD,
               area.getWidth()-(int)iconSize-8-70, 36,
               juce::Justification::centredLeft);

    const int count = chain.getPluginCount();
    if (count > 0)
    {
        juce::Font bf(SL::FONT_MICRO, juce::Font::bold);
        const juce::String cs = juce::String(count)+(count==1?" plugin":" plugins");
        const float bw = bf.getStringWidthFloat(cs)+12, bh = 16;
        const float bx = area.getX()+(int)iconSize+8
                         + juce::Font(SL::FONT_HEADING,juce::Font::bold).getStringWidthFloat("Plugin Chain")+8;
        const float by = SL::PAD+(36.0f-bh)*.5f;
        g.setColour(SL::ACCENT.withAlpha(0.14f));
        g.fillRoundedRectangle(bx,by,bw,bh,bh*.5f);
        g.setColour(SL::ACCENT.withAlpha(0.4f));
        g.drawRoundedRectangle(bx+.5f,by+.5f,bw-1,bh-1,(bh-1)*.5f,1.0f);
        g.setColour(SL::ACCENT);
        g.setFont(bf);
        g.drawText(cs,(int)bx,(int)by,(int)bw,(int)bh,juce::Justification::centred);
    }

    g.setColour(SL::BORDER.withAlpha(0.8f));
    g.fillRect(area.getX(), area.getY()+36+SL::PAD_SM, area.getWidth(), 1);

    if (chain.getPluginCount() == 0)
    {
        const auto listBounds = dragList->getBounds();
        const float iconSz  = 40.0f;
        const float iconX   = listBounds.getX() + (listBounds.getWidth()  - iconSz) * 0.5f;
        const float iconY   = listBounds.getY() + (listBounds.getHeight() - iconSz - 36) * 0.5f;

        LucideIcons::draw(g, "puzzle",
                          { iconX, iconY, iconSz, iconSz },
                          SL::TEXT_MUTED.withAlpha(0.25f));

        g.setColour(SL::TEXT_MUTED.withAlpha(0.55f));
        g.setFont(juce::Font(SL::FONT_BODY));
        g.drawText("No plugins added yet",
                   0, (int)(iconY + iconSz + 10),
                   getWidth(), 20, juce::Justification::centred);

        g.setColour(SL::TEXT_MUTED.withAlpha(0.35f));
        g.setFont(juce::Font(SL::FONT_CAPTION));
        g.drawText("Press + to add a VST plugin",
                   0, (int)(iconY + iconSz + 32),
                   getWidth(), 16, juce::Justification::centred);
    }
}

void PluginChainPanel::resized()
{
    using SL = SlopLookAndFeel;
    auto area = getLocalBounds().reduced(SL::PAD);

    auto header = area.removeFromTop(36);
    scanButton.setBounds(header.removeFromRight(30).withSizeKeepingCentre(30, 30));
    header.removeFromRight(SL::PAD_XS);
    addButton .setBounds(header.removeFromRight(30).withSizeKeepingCentre(30, 30));

    area.removeFromTop(SL::PAD_SM + 1);

    const int bottomH = 16 + SL::PAD_SM + 16;
    dragList->setBounds(area.removeFromTop(area.getHeight() - bottomH - SL::PAD_SM));

    area.removeFromTop(SL::PAD_SM);
    hintLabel  .setBounds(area.removeFromTop(16));
    area.removeFromTop(SL::PAD_SM);
    scanStatus .setBounds(area.removeFromTop(16));
}

void PluginChainPanel::refreshList()
{
    dragList->refresh();
    repaint();
}

void PluginChainPanel::onAddPlugin()
{
    const auto& known = chain.getKnownPlugins();
    if (known.getNumTypes() == 0)
    {
        juce::AlertWindow::showMessageBoxAsync(
            juce::AlertWindow::InfoIcon, "No Plugins",
            "Scan for VST3 plugins first (scan-search button).");
        return;
    }

    juce::PopupMenu menu;
    for (int i = 0; i < known.getNumTypes(); ++i)
        menu.addItem(i+1, known.getType(i)->name);

    menu.showMenuAsync(
        juce::PopupMenu::Options().withTargetComponent(&addButton),
        [this, &known](int result)
        {
            if (result <= 0) return;
            const auto* desc = known.getType(result-1);
            if (!desc) return;
            juce::String err;
            if (!chain.addPlugin(*desc, 44100.0, 512, err))
                juce::AlertWindow::showMessageBoxAsync(
                    juce::AlertWindow::WarningIcon, "Plugin Load Error", err);
        });
}

void PluginChainPanel::onScanPlugins()
{
    scanStatus.setText("Scanning...", juce::dontSendNotification);
    scanButton.setEnabled(false);
    juce::Thread::launch([this]
    {
        chain.scanPlugins([](int,int){});
        juce::MessageManager::callAsync([this]
        {
            scanStatus.setText(
                juce::String(chain.getKnownPlugins().getNumTypes()) + " plugins found",
                juce::dontSendNotification);
            scanButton.setEnabled(true);
        });
    });
}

void PluginChainPanel::onRemovePlugin(int index)
{
    if (index < 0 || index >= chain.getPluginCount()) return;
    if (openEditors.count(index) && openEditors[index])
    {
        openEditors[index]->closeButtonPressed();
        openEditors.erase(index);
    }
    chain.removePlugin(index);
    if (dragList->selectedRow >= chain.getPluginCount())
        dragList->selectedRow = chain.getPluginCount() - 1;
    dragList->refresh();
    repaint();
}
