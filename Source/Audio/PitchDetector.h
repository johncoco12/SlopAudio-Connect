#pragma once
#include <JuceHeader.h>
#include <atomic>
#include <vector>

class PitchDetector
{
public:
    struct Result
    {
        float frequency  = 0.0f;
        float confidence = 0.0f;
        bool  valid      = false;
    };

    explicit PitchDetector(int windowSize = 4096);

    Result process(const float* samples, int numSamples, float sampleRate);
    void reset();

    void setThreshold(float t)  { threshold.store(t); }
    void setMinFreq  (float hz) { minFreq.store(hz);  }
    void setMaxFreq  (float hz) { maxFreq.store(hz);  }

    float getThreshold() const { return threshold.load(); }
    float getMinFreq()   const { return minFreq.load();   }
    float getMaxFreq()   const { return maxFreq.load();   }

private:
    Result runYin(float sampleRate) const;

    int              windowSize;
    int              hopSize;
    std::vector<float> accumBuf;
    int              accumCount = 0;
    mutable std::vector<float> yinBuf;

    std::atomic<float> threshold { 0.10f };
    std::atomic<float> minFreq   { 40.0f };
    std::atomic<float> maxFreq   { 2000.0f };
};
