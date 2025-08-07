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
#include "SwappableProcessor.h"
class FlangerProcessor : public SwappableProcessor {
public:
    //FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts, int index);
    FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex);
    ~FlangerProcessor() {}

    float getDelay() { return *delayParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getMix() { return *mixParam; }

    void setOnState(bool value);
    void setDelay(float value);
    void setMix(float value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double sampleRate, int _totalNumInputChannels) override {
        totalNumInputChannels = totalNumInputChannels;
        sampleRate = static_cast<float>(sampleRate); 
        int maxindex = static_cast<int>(sampleRate * DELAY_MAX);
        sampleBuffer.prepare(totalNumInputChannels, maxindex);
    };
    void moveParamValues(int index) override;
    void assignParamPointers(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;
private:
    juce::AudioProcessorValueTreeState& apvts;

    std::atomic<float>* delayParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* mixParam;

    CircularBuffer sampleBuffer;
    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
};


