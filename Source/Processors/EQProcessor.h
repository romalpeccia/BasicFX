/*
  ==============================================================================

    EQProcessor.h
    Created: 6 May 2025 9:43:32pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "SwappableProcessor.h"
class EQProcessor : public SwappableProcessor {
public:
    EQProcessor(juce::AudioProcessorValueTreeState& _apvts, int index);
    ~EQProcessor() {}

    float getAmount() { return *amountParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getEQType() { return *eqTypeParam; }
    void setOnState(bool value);
    void setAmount(float value);
    void setEQType(int value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) override { }
    void assignParamPointers(int index) override;
    void moveParamValues(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;

private:
    juce::AudioProcessorValueTreeState& apvts;


    std::atomic<float>* amountParam;
    std::atomic<float>* onStateParam; //TODO: why does this not work as a bool? why does it work as a float? weird
    std::atomic<float>* eqTypeParam;


};