/*
  ==============================================================================

    FlangerProcessor.h
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "../CircularBuffer.h"
class FlangerProcessor {
public:
    FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts);
    ~FlangerProcessor() {}
    float getDelay() { return *delayParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getMix() { return *mixParam; }
    void processBlock(juce::AudioBuffer<float>& buffer);
    void prepareToPlay(double sampleRate, int _totalNumInputChannels) {
        totalNumInputChannels = totalNumInputChannels;
        sampleRate = static_cast<float>(sampleRate); 
        int maxDelaySamples = static_cast<int>(sampleRate * DELAY_MAX);
        sampleBuffer.prepare(totalNumInputChannels, maxDelaySamples);
    };
private:
    juce::AudioProcessorValueTreeState& apvts;

    std::atomic<float>* delayParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* mixParam;

    CircularBuffer sampleBuffer;
    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
};


