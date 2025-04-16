/*
  ==============================================================================

    VisualizerProcessor.h
    Created: 15 Apr 2025 1:52:15pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../CircularBuffer.h"
#include "../Utilities.h"

class VisualizerProcessor {
public:
    VisualizerProcessor(int _maxSamples);
    VisualizerProcessor::~VisualizerProcessor() {}

    void prepareToPlay(int sampleRate, int _totalNumInputChannels);
    void processBlock(juce::AudioBuffer<float>& buffer);

    float getCurrentValue();
    void setMaxSamples(int _maxSamples);
    float readSample(int channel, int indexFromLatest) const;

private:
    CircularBuffer sampleBuffer;
    float currentValue = 0;
    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
    int maxSamples;
    const int DOWNSAMPLE_FACTOR = 64;
    //TODO figure out better value for this and REFRESH_RATE_MS in component. both contribute to how fast the signal scrolls, whether the buffer is getting polled more often or the buffer is getting filled more
};