/*
  ==============================================================================

    DBMeterProcessor.h
    Created: 15 Apr 2025 12:23:35am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../CircularBuffer.h"
#include "../Utilities.h"

class DBMeterProcessor {
    public:
        DBMeterProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) {
            float peak = juce::jmax(buffer.getMagnitude(0, 0, buffer.getNumSamples()), buffer.getMagnitude(1, 0, buffer.getNumSamples()));
            db = juce::Decibels::gainToDecibels(peak);
        }
        float getDB() { return db; }
    private:
        float db = -100.f;
};








/*
class VisualizerProcessor {
    //WIP
    
    VisualizerProcessor(int _maxSamples) : maxSamples(_maxSamples) {}
    void prepareToPlay(int _totalNumInputChannels) {
        totalNumInputChannels = totalNumInputChannels;
        sampleBuffer.prepare(totalNumInputChannels, maxSamples);
    };
    void processBlock(juce::AudioBuffer<float>& buffer) {
        float rms = calculateRMSAcrossChannels(buffer);
        sampleBuffer.write(1, rms);
        
    }
    void getValue(int index) {
        sampleBuffer.readInterpolated(1, index);
    }
private:
    std::atomic<float> currentLevelDB = -100.0f;
    CircularBuffer sampleBuffer;
    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
    int maxSamples;

};
*/