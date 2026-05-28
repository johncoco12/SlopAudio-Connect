#include "AudioEngine.h"
#include <cmath>

AudioEngine::AudioEngine() = default;

AudioEngine::~AudioEngine()
{
    shutdown();
}

void AudioEngine::initialise()
{
    deviceManager.initialiseWithDefaultDevices(2, 2);
    deviceManager.addAudioCallback(this);
}

void AudioEngine::shutdown()
{
    deviceManager.removeAudioCallback(this);
    deviceManager.closeAudioDevice();
    pluginChain.releaseResources();
}

void AudioEngine::setMonitoring(bool active)
{
    if (!active)
        pitchDetector.reset();
    monitoring.store(active);
}

bool AudioEngine::readPitchResult(PitchResult& out)
{
    const int available = abstractFifo.getNumReady();
    if (available == 0)
        return false;

    int start1, size1, start2, size2;
    abstractFifo.prepareToRead(1, start1, size1, start2, size2);
    if (size1 > 0) out = pitchFifo[start1];
    abstractFifo.finishedRead(size1 + size2);
    return size1 > 0;
}

void AudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    currentSampleRate = static_cast<float>(device->getCurrentSampleRate());
    const int blockSize = device->getCurrentBufferSizeSamples();
    pluginChain.prepare(device->getCurrentSampleRate(), blockSize);
    pitchDetector.reset();
    processingBuffer.setSize(2, blockSize);
}

void AudioEngine::audioDeviceStopped()
{
    pluginChain.releaseResources();
    pitchDetector.reset();
}

void AudioEngine::audioDeviceError(const juce::String&) {}

void AudioEngine::audioDeviceIOCallbackWithContext(
    const float* const* inputChannelData,  int numInputChannels,
    float* const* outputChannelData,       int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&)
{

    for (int ch = 0; ch < numOutputChannels; ++ch)
        if (outputChannelData[ch])
            juce::FloatVectorOperations::clear(outputChannelData[ch], numSamples);

    const bool pitchActive  = monitoring.load();
    const bool outputActive = outputMonitoring.load();
    if (!pitchActive && !outputActive)
        return;

    const int bufChans = std::max(numInputChannels, 2);
    processingBuffer.setSize(bufChans, numSamples, false, false, true);

    for (int ch = 0; ch < numInputChannels; ++ch)
        if (inputChannelData[ch])
            processingBuffer.copyFrom(ch, 0, inputChannelData[ch], numSamples);

    if (numInputChannels == 1 && bufChans >= 2)
        processingBuffer.copyFrom(1, 0, processingBuffer.getReadPointer(0), numSamples);

    if (pitchActive && numInputChannels > 0 && inputChannelData[0])
    {
        auto result = pitchDetector.process(inputChannelData[0],
                                            numSamples, currentSampleRate);
        if (result.valid && result.confidence >= minConfidence.load())
        {
            PitchResult pr;
            pr.frequency  = result.frequency;
            pr.confidence = result.confidence;
            pr.midiNote   = freqToMidi(result.frequency);
            pr.noteName   = midiToNoteName(pr.midiNote);

            int start1, size1, start2, size2;
            abstractFifo.prepareToWrite(1, start1, size1, start2, size2);
            if (size1 > 0) pitchFifo[start1] = pr;
            abstractFifo.finishedWrite(size1 + size2);
        }
    }

    juce::MidiBuffer midi;
    pluginChain.processBlock(processingBuffer, midi);

    if (outputActive)
    {
        const int outChans = std::min(processingBuffer.getNumChannels(),
                                      numOutputChannels);
        for (int ch = 0; ch < outChans; ++ch)
            if (outputChannelData[ch])
                juce::FloatVectorOperations::copy(outputChannelData[ch],
                                                  processingBuffer.getReadPointer(ch),
                                                  numSamples);
    }
}

int AudioEngine::freqToMidi(float freq)
{
    return static_cast<int>(std::round(69.0f + 12.0f * std::log2(freq / 440.0f)));
}

juce::String AudioEngine::midiToNoteName(int midi)
{
    static const char* names[] = {
        "C","C#","D","D#","E","F","F#","G","G#","A","A#","B"
    };
    const int octave = (midi / 12) - 1;
    return juce::String(names[midi % 12]) + juce::String(octave);
}
