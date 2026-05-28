#include "PitchDetector.h"
#include <cmath>
#include <numeric>

PitchDetector::PitchDetector(int ws)
    : windowSize(ws), hopSize(ws / 2),
      accumBuf(ws, 0.0f), yinBuf(ws / 2, 0.0f)
{}

void PitchDetector::reset()
{
    std::fill(accumBuf.begin(), accumBuf.end(), 0.0f);
    accumCount = 0;
}

PitchDetector::Result PitchDetector::process(const float* samples,
                                              int numSamples,
                                              float sampleRate)
{
    Result latest;
    int consumed = 0;

    while (consumed < numSamples)
    {
        int canFill = windowSize - accumCount;
        int filling = std::min(canFill, numSamples - consumed);
        std::memcpy(accumBuf.data() + accumCount, samples + consumed,
                    filling * sizeof(float));
        accumCount += filling;
        consumed   += filling;

        if (accumCount == windowSize)
        {
            latest = runYin(sampleRate);

            std::memmove(accumBuf.data(), accumBuf.data() + hopSize,
                         hopSize * sizeof(float));
            accumCount = hopSize;
        }
    }
    return latest;
}

PitchDetector::Result PitchDetector::runYin(float sampleRate) const
{
    const int half   = windowSize / 2;
    const float* buf = accumBuf.data();

    yinBuf[0]        = 1.0f;
    float runningSum = 0.0f;

    for (int tau = 1; tau < half; ++tau)
    {
        float d = 0.0f;
        for (int j = 0; j < half; ++j)
        {
            float diff = buf[j] - buf[j + tau];
            d += diff * diff;
        }
        runningSum += d;
        yinBuf[tau] = (runningSum > 0.0f)
                          ? (d * static_cast<float>(tau) / runningSum)
                          : 1.0f;
    }

    const float thr = threshold.load();
    int tauMin = -1;
    for (int tau = 2; tau < half - 1; ++tau)
    {
        if (yinBuf[tau] < thr)
        {
            while (tau + 1 < half - 1 && yinBuf[tau + 1] < yinBuf[tau])
                ++tau;
            tauMin = tau;
            break;
        }
    }

    if (tauMin <= 0)
        return {};

    float betterTau;
    if (tauMin > 1 && tauMin < half - 1)
    {
        float s0   = yinBuf[tauMin - 1];
        float s1   = yinBuf[tauMin];
        float s2   = yinBuf[tauMin + 1];
        float dnom = 2.0f * (s0 - 2.0f * s1 + s2);
        betterTau  = (dnom != 0.0f)
                         ? static_cast<float>(tauMin) + (s0 - s2) / dnom
                         : static_cast<float>(tauMin);
    }
    else
    {
        betterTau = static_cast<float>(tauMin);
    }

    const float freq = sampleRate / betterTau;
    if (freq < minFreq.load() || freq > maxFreq.load())
        return {};

    return { freq, 1.0f - yinBuf[tauMin], true };
}
