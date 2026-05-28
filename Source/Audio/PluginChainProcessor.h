#pragma once
#include <JuceHeader.h>
#include <vector>
#include <memory>

class PluginChainProcessor
{
public:
    PluginChainProcessor();

    void scanPlugins(std::function<void(int, int)> progressCallback = nullptr);

    const juce::KnownPluginList& getKnownPlugins() const { return knownPlugins; }

    bool addPlugin(const juce::PluginDescription& desc,
                   double sampleRate, int blockSize, juce::String& error);

    void removePlugin(int index);
    void movePlugin(int fromIndex, int toIndex);
    int  getPluginCount() const;
    juce::AudioPluginInstance* getPlugin(int index);

    void setBypassed(int index, bool bypassed);
    bool isBypassed(int index) const;

    void prepare(double sampleRate, int maxBlockSize);
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midi);
    void releaseResources();

    juce::ValueTree saveState() const;
    void            loadState(const juce::ValueTree& state,
                              double sampleRate, int blockSize);

    std::function<void()> onChainChanged;

private:
    juce::AudioPluginFormatManager              formatManager;
    juce::KnownPluginList                       knownPlugins;
    std::vector<std::unique_ptr<juce::AudioPluginInstance>> chain;
    std::vector<bool>                           bypassedFlags;

    double currentSampleRate = 44100.0;
    int    currentBlockSize  = 512;

    juce::CriticalSection chainLock;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginChainProcessor)
};
