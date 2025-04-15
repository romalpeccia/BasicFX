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
        VisualizerProcessor() {};
        VisualizerProcessor(int _maxSamples) : maxSamples(_maxSamples) {}
        ~VisualizerProcessor() {};

        void prepareToPlay(int _totalNumInputChannels) { //TODO: is this doing anything?
            totalNumInputChannels = totalNumInputChannels;
            sampleBuffer.prepare(totalNumInputChannels, maxSamples);
        };
        void processBlock(juce::AudioBuffer<float>& buffer) {
            //currentValue = calculateAverageAcrossChannels(buffer);
            int numChannels = buffer.getNumChannels();
            const float* channelData = buffer.getReadPointer(1);
            currentValue = channelData[0];
        }
        float getCurrentValue() { return currentValue; }
        void setMaxSamples(int _maxSamples) { 
            maxSamples = _maxSamples; 
            sampleBuffer.prepare(totalNumInputChannels, maxSamples); 
        }

        CircularBuffer sampleBuffer;//TODO: make this private and return a pointer
    private:
        float currentValue = 0;
        float sampleRate = 44100.0f;
        int totalNumInputChannels = 2;
        int maxSamples;

};
