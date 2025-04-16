/*
  ==============================================================================

    VisualizerProcessor.cpp
    Created: 15 Apr 2025 1:52:15pm
    Author:  romal

  ==============================================================================
*/

#include "VisualizerProcessor.h"

VisualizerProcessor::VisualizerProcessor(int _maxSamples) : maxSamples(_maxSamples) {
    sampleBuffer.prepare(1, maxSamples);
}

void VisualizerProcessor::prepareToPlay(int sampleRate, int _totalNumInputChannels) {
    sampleBuffer.prepare(1, maxSamples);
    totalNumInputChannels = _totalNumInputChannels;
}

void VisualizerProcessor::processBlock(juce::AudioBuffer<float>& buffer) {
    int numSamples = buffer.getNumSamples();

    float* channelData[MAX_INPUT_CHANNELS];
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        channelData[channel] = buffer.getWritePointer(channel);
    }

    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        if (sampleNum % DOWNSAMPLE_FACTOR == 0) {
            float sample = 0.f;
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                sample += channelData[channel][sampleNum]; 
            }
            sample /= totalNumInputChannels; //TODO: average across channels? better way to do this?

            sampleBuffer.write(0, sample);
        }
    }
}

float VisualizerProcessor::getCurrentValue() {
    return currentValue;
}

void VisualizerProcessor::setMaxSamples(int _maxSamples) {
    maxSamples = _maxSamples;
    sampleBuffer.prepare(1, maxSamples);
}

float VisualizerProcessor::readSample(int channel, int indexFromLatest) const {
    return sampleBuffer.read(channel, indexFromLatest);
}