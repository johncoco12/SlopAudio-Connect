#pragma once
#include <JuceHeader.h>
#include "../Audio/PluginChainProcessor.h"
#include "SlopButton.h"

class PluginEditorWindow : public juce::DocumentWindow
{
public:
    PluginEditorWindow(juce::AudioPluginInstance* plugin,
                       std::function<void()> onClose)
        : juce::DocumentWindow(plugin->getName(),
                               juce::Colour(0xff1e1e1e),
                               DocumentWindow::closeButton),
          closeCb(std::move(onClose))
    {
        setUsingNativeTitleBar(true);

        auto* editor = plugin->createEditorIfNeeded();
        if (editor)
            setContentOwned(editor, true);
        else
            setContentOwned(new juce::GenericAudioProcessorEditor(*plugin), true);

        setResizable(true, false);
        centreWithSize(getWidth(), getHeight());
        setVisible(true);
        toFront(true);
    }

    void closeButtonPressed() override
    {
        if (closeCb) closeCb();
        delete this;
    }

private:
    std::function<void()> closeCb;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditorWindow)
};

class PluginChainPanel : public juce::Component
{
public:
    explicit PluginChainPanel(PluginChainProcessor& chain);
    ~PluginChainPanel() override;
    void resized() override;
    void paint(juce::Graphics& g) override;

private:
    void refreshList();
    void openEditorFor(int index);
    void onAddPlugin();
    void onScanPlugins();
    void onRemovePlugin(int index);

    PluginChainProcessor& chain;

    struct DragList;
    std::unique_ptr<DragList> dragList;

    SlopButton addButton;
    SlopButton scanButton;

    juce::Label scanStatus;
    juce::Label hintLabel;

    std::map<int, PluginEditorWindow*> openEditors;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginChainPanel)
};
