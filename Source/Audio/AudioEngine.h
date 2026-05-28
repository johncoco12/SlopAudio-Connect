#pragma once
#include <JuceHeader.h>
#include "PitchDetector.h"
#include "PluginChainProcessor.h"
#include <array>
#include <atomic>

class AudioEngine : public juce::AudioIODeviceCallback
{
public:
    struct PitchResult
    {
        float frequency  = 0.0f;
        float confidence = 0.0f;
        int   midiNote   = 0;
        juce::String noteName;
    };

    AudioEngine();
    ~AudioEngine() override;

    void initialise();
    void shutdown();

    juce::AudioDeviceManager& getDeviceManager() { return deviceManager; }
    PluginChainProcessor&     getPluginChain()   { return pluginChain; }

    void setMonitoring(bool active);
    bool isMonitoring() const { return monitoring.load(); }

    void setOutputMonitoring(bool active) { outputMonitoring.store(active); }
    bool isOutputMonitoring() const       { return outputMonitoring.load(); }

    void  setYinThreshold (float t)  { pitchDetector.setThreshold(t); }
    void  setMinFreq      (float hz) { pitchDetector.setMinFreq(hz);  }
    void  setMaxFreq      (float hz) { pitchDetector.setMaxFreq(hz);  }
    void  setMinConfidence(float c)  { minConfidence.store(c);        }

    float getYinThreshold ()  const { return pitchDetector.getThreshold(); }
    float getMinFreq      ()  const { return pitchDetector.getMinFreq();   }
    float getMaxFreq      ()  const { return pitchDetector.getMaxFreq();   }
    float getMinConfidence()  const { return minConfidence.load();         }

    bool readPitchResult(PitchResult& out);

    void audioDeviceIOCallbackWithContext(
        const float* const* inputChannelData,  int numInputChannels,
        float* const* outputChannelData,       int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;
    void audioDeviceError(const juce::String& errorMessage) override;

private:
    static int   freqToMidi(float freq);
    static juce::String midiToNoteName(int midi);

    static constexpr int kFifoSize = 512;

    juce::AudioDeviceManager deviceManager;
    PluginChainProcessor     pluginChain;
    PitchDetector            pitchDetector { 4096 };

    std::atomic<bool>  monitoring      { false };
    std::atomic<bool>  outputMonitoring { false };
    std::atomic<float> minConfidence   { 0.0f  };
    float              currentSampleRate = 44100.0f;

    std::array<PitchResult, kFifoSize> pitchFifo;
    juce::AbstractFifo                 abstractFifo { kFifoSize };

    juce::AudioBuffer<float> processingBuffer;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioEngine)
};
