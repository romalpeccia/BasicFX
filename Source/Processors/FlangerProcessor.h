/*
  ==============================================================================

    FlangerProcessor.h
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"


class CircularBuffer
{
public:
    CircularBuffer() = default;

    void prepare(int numChannels, int maxDelaySamples)
    {
        buffers.resize(numChannels);
        writeIndices.resize(numChannels);

        for (int ch = 0; ch < numChannels; ++ch)
        {
            buffers[ch].resize(maxDelaySamples, 0.0f);
            writeIndices[ch] = 0;
        }
        size = maxDelaySamples;
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

class FlangerProcessor {
public:
    FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts);
    ~FlangerProcessor() {}
    float getDelay() { return *delayParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getMix() { return *mixParam; }
    void processBlock(juce::AudioBuffer<float>& buffer);
    void prepareToPlay(double sampleRate, int totalNumInputChannels) {
        sampleRate = static_cast<float>(sampleRate); 
        int maxDelaySamples = static_cast<int>(sampleRate * DELAY_MAX); // 2 seconds
        delayBuffer.prepare(totalNumInputChannels, maxDelaySamples);
    };
private:
    juce::AudioProcessorValueTreeState& apvts;

    std::atomic<float>* delayParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* mixParam;

    CircularBuffer delayBuffer;
    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
};


