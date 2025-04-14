/*
  ==============================================================================

    FlangerProcessor.h
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"

class FlangerProcessor {
public:
    FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts);
    ~FlangerProcessor() {}
    float getDelay() { return *delayParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getMix() { return *mixParam; }
    void processBlock(juce::AudioBuffer<float>& buffer, int totalNumInputChannels, int sampleRate);
private:
    juce::AudioProcessorValueTreeState& apvts;

    std::atomic<float>* delayParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* mixParam;

    float delayBuffer[MAX_INPUT_CHANNELS][MAX_BUFFER_SIZE] = { 0 };
};