/*
  ==============================================================================

    CircularBuffer.cpp
    Created: 15 Apr 2025 12:17:17am
    Author:  romal

  ==============================================================================
*/

#include <JuceHeader.h>
#include "CircularBuffer.h"



void CircularBuffer::prepare(int numChannels, int maxSamples)
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

void CircularBuffer::write(int channel, float sample)
{
    buffers[channel][writeIndices[channel]] = sample;
    writeIndices[channel] = (writeIndices[channel] + 1) % size;
}

float CircularBuffer::read(int channel, int index) const
{
    int readIndex = writeIndices[channel] - index;
    if (readIndex < 0)
        readIndex += size;

    return buffers[channel][readIndex];
}

float CircularBuffer::readInterpolated(int channel, float index) const
{
    float readIndex = static_cast<float>(writeIndices[channel]) - index;

    if (readIndex < 0.0f)
        readIndex += static_cast<float>(size);

    int indexA = static_cast<int>(std::floor(readIndex)) % size;
    int indexB = (indexA + 1) % size;
    float frac = readIndex - static_cast<float>(indexA);

    float sampleA = buffers[channel][indexA];
    float sampleB = buffers[channel][indexB];

    return (1.0f - frac) * sampleA + frac * sampleB;
}