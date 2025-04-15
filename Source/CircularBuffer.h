/*
  ==============================================================================

    CircularBuffer.h
    Created: 15 Apr 2025 12:17:17am
    Author:  romal

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/


class CircularBuffer
{
public:
    CircularBuffer() = default;

    void prepare(int numChannels, int maxSamples)
    {
        buffers.resize(numChannels);
        writeIndices.resize(numChannels);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            buffers[ch].resize(maxSamples, 0.0f);
            writeIndices[ch] = 0;
        }
        size = maxSamples;
    }

    void write(int channel, float sample)
    {
        buffers[channel][writeIndices[channel]] = sample;
        writeIndices[channel] = (writeIndices[channel] + 1) % size;
    }

    float read(int channel, int delaySamples) const
    {
        int readIndex = writeIndices[channel] - delaySamples;
        if (readIndex < 0)
            readIndex += size;

        return buffers[channel][readIndex];
    }

    float readInterpolated(int channel, float delaySamples) const
    {
        float readIndex = static_cast<float>(writeIndices[channel]) - delaySamples;

        if (readIndex < 0.0f)
            readIndex += static_cast<float>(size);

        // Wrap using integer math
        int indexA = static_cast<int>(std::floor(readIndex)) % size;
        int indexB = (indexA + 1) % size;
        float frac = readIndex - static_cast<float>(indexA);

        float sampleA = buffers[channel][indexA];
        float sampleB = buffers[channel][indexB];

        // Linear interpolation
        return (1.0f - frac) * sampleA + frac * sampleB;
    }
private:
    std::vector<std::vector<float>> buffers;
    std::vector<int> writeIndices;
    int size = 0;
};
