/*
  ==============================================================================

    UtilityProcessor.h
    Created: 19 Aug 2025 5:27:13pm
    Author:  romal

  ==============================================================================
*/

#pragma once

#pragma once
#include "../Utilities.h"
#include "SwappableProcessor.h"
class UtilityProcessor : public SwappableProcessor {
public:

    UtilityProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex);
    ~UtilityProcessor() {}

    bool getOnState() { return bool(*onStateParam); }
    float getVolume() { return *volumeParam; }
    float getStereo() { return *stereoParam; }
    void setOnState(bool value);
    void setVolume(float value);
    void setStereo(float value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) override { sampleRate = _sampleRate, totalNumInputChannels = _totalNumInputChannels; }

    void assignParamPointers(int index) override;
    void moveParamValues(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    int totalNumInputChannels = 2;
    float sampleRate = 44100.f;

    std::atomic<float>* onStateParam; //TODO: why does this not work as a bool? why does it work as a float? weird
    std::atomic<float>* volumeParam;
    std::atomic<float>* stereoParam;

};