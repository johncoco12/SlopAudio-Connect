#include "PluginChainProcessor.h"

PluginChainProcessor::PluginChainProcessor()
{
    formatManager.addDefaultFormats();
}

void PluginChainProcessor::scanPlugins(std::function<void(int, int)> progressCallback)
{
    juce::FileSearchPath searchPaths;
    searchPaths.add(juce::File::getSpecialLocation(juce::File::userHomeDirectory)
                        .getChildFile(".vst3"));
    searchPaths.add(juce::File("/usr/lib/vst3"));
    searchPaths.add(juce::File("/usr/local/lib/vst3"));

    juce::PluginDirectoryScanner scanner(
        knownPlugins, *formatManager.getFormat(0),
        searchPaths, true, juce::File{}, false);

    juce::String name;
    int done = 0;

    while (scanner.scanNextFile(true, name))
    {
        if (progressCallback)
            progressCallback(++done, 0);
    }
}

bool PluginChainProcessor::addPlugin(const juce::PluginDescription& desc,
                                     double sampleRate, int blockSize,
                                     juce::String& error)
{
    auto instance = formatManager.createPluginInstance(desc, sampleRate, blockSize, error);
    if (!instance)
        return false;

    instance->prepareToPlay(sampleRate, blockSize);

    {
        juce::ScopedLock sl(chainLock);
        chain.push_back(std::move(instance));
        bypassedFlags.push_back(false);
    }

    if (onChainChanged)
        onChainChanged();

    return true;
}

void PluginChainProcessor::removePlugin(int index)
{
    juce::ScopedLock sl(chainLock);
    if (index >= 0 && index < static_cast<int>(chain.size()))
    {
        chain[index]->releaseResources();
        chain.erase(chain.begin() + index);
        bypassedFlags.erase(bypassedFlags.begin() + index);
        if (onChainChanged) onChainChanged();
    }
}

void PluginChainProcessor::movePlugin(int from, int to)
{
    juce::ScopedLock sl(chainLock);
    const int n = static_cast<int>(chain.size());
    if (from < 0 || from >= n || to < 0 || to >= n || from == to)
        return;

    auto plugin = std::move(chain[from]);
    chain.erase(chain.begin() + from);
    chain.insert(chain.begin() + to, std::move(plugin));

    bool wasBy = bypassedFlags[from];
    bypassedFlags.erase(bypassedFlags.begin() + from);
    bypassedFlags.insert(bypassedFlags.begin() + to, wasBy);

    if (onChainChanged) onChainChanged();
}

int PluginChainProcessor::getPluginCount() const
{
    return static_cast<int>(chain.size());
}

juce::AudioPluginInstance* PluginChainProcessor::getPlugin(int index)
{
    if (index >= 0 && index < static_cast<int>(chain.size()))
        return chain[index].get();
    return nullptr;
}

void PluginChainProcessor::setBypassed(int index, bool bypassed)
{
    juce::ScopedLock sl(chainLock);
    if (index >= 0 && index < static_cast<int>(bypassedFlags.size()))
        bypassedFlags[index] = bypassed;
    if (onChainChanged) onChainChanged();
}

bool PluginChainProcessor::isBypassed(int index) const
{
    juce::ScopedLock sl(chainLock);
    if (index >= 0 && index < static_cast<int>(bypassedFlags.size()))
        return bypassedFlags[index];
    return false;
}

void PluginChainProcessor::prepare(double sampleRate, int maxBlockSize)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = maxBlockSize;

    juce::ScopedLock sl(chainLock);
    for (auto& p : chain)
        p->prepareToPlay(sampleRate, maxBlockSize);
}

void PluginChainProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& midi)
{
    juce::ScopedLock sl(chainLock);
    for (int i = 0; i < static_cast<int>(chain.size()); ++i)
    {
        if (i < static_cast<int>(bypassedFlags.size()) && bypassedFlags[i])
            continue;
        chain[i]->processBlock(buffer, midi);
    }
}

void PluginChainProcessor::releaseResources()
{
    juce::ScopedLock sl(chainLock);
    for (auto& p : chain)
        p->releaseResources();
}

juce::ValueTree PluginChainProcessor::saveState() const
{
    juce::ValueTree root("PluginChain");
    juce::ScopedLock sl(chainLock);

    for (auto& p : chain)
    {
        juce::MemoryBlock state;
        p->getStateInformation(state);

        juce::ValueTree node("Plugin");
        node.setProperty("identifier", p->getPluginDescription().createIdentifierString(), nullptr);
        node.setProperty("state", state.toBase64Encoding(), nullptr);
        root.appendChild(node, nullptr);
    }
    return root;
}

void PluginChainProcessor::loadState(const juce::ValueTree& state,
                                      double sampleRate, int blockSize)
{
    {
        juce::ScopedLock sl(chainLock);
        for (auto& p : chain) p->releaseResources();
        chain.clear();
    }

    for (int i = 0; i < state.getNumChildren(); ++i)
    {
        auto node = state.getChild(i);
        juce::PluginDescription desc;

        const juce::String identifier = node.getProperty("identifier").toString();
        for (int j = 0; j < knownPlugins.getNumTypes(); ++j)
        {
            if (knownPlugins.getType(j)->createIdentifierString() == identifier)
            {
                desc = *knownPlugins.getType(j);
                break;
            }
        }

        juce::String err;
        auto instance = formatManager.createPluginInstance(desc, sampleRate, blockSize, err);
        if (!instance)
            continue;

        juce::MemoryBlock stateData;
        stateData.fromBase64Encoding(node.getProperty("state").toString());
        instance->setStateInformation(stateData.getData(),
                                      static_cast<int>(stateData.getSize()));
        instance->prepareToPlay(sampleRate, blockSize);

        juce::ScopedLock sl(chainLock);
        chain.push_back(std::move(instance));
    }

    if (onChainChanged) onChainChanged();
}
