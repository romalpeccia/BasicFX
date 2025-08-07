/*
  ==============================================================================

    GateProcessor.h
    Created: 13 Apr 2025 11:04:40pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "SwappableProcessor.h"
class GateProcessor : public SwappableProcessor {
public:
    //GateProcessor(juce::AudioProcessorValueTreeState& _apvts, int index);
    GateProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex);
    ~GateProcessor() {}

    float getThreshold() { return *thresholdParam; }
    int getGateType() { return int(*gateTypeParam); }
    bool getOnState() { return bool(*onStateParam); }
    float getAttack() { return *attackParam; }
    float getRelease() { return *releaseParam; }
    float getHold() { return *holdParam; }

    void setOnState(bool value);
    void setThreshold(float value);
    void setGateType(int value);
    void setAttack(float value);
    void setRelease(float value);
    void setHold(float value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) override { sampleRate = _sampleRate, totalNumInputChannels = _totalNumInputChannels; }
    void assignParamPointers(int index) override;
    void moveParamValues(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;

private:
    juce::AudioProcessorValueTreeState& apvts;
    int totalNumInputChannels = 2;
    float sampleRate = 44100.f;
    
    std::atomic<float>* thresholdParam;
    std::atomic<float>* gateTypeParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* attackParam;
    std::atomic<float>* releaseParam;
    std::atomic<float>* holdParam;

    void processGateSimple(juce::AudioBuffer<float>& buffer);
    void processGateMedium(juce::AudioBuffer<float>& buffer);
    void processGateAdvanced(juce::AudioBuffer<float>& buffer);

    enum class GateState {
        Open,
        Attack,
        Hold,
        Release
    };

    GateState gateState = GateState::Open;
    int attackSampleCounter = 0, releaseSampleCounter = 0, holdSampleCounter = 0;
    int retriggerHoldSampleCounter = 0; //WIP for when retrigger happens during hold phase 
    float attackStartRatio = 1.f; //WIP for when retrigger happens during attack phase 
};

