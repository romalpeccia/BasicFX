/*
  ==============================================================================

    GateProcessor.h
    Created: 13 Apr 2025 11:04:40pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "Utilities.h"

class GateProcessor {
public:
    GateProcessor(juce::AudioProcessorValueTreeState& _apvts);
    ~GateProcessor() {}
    float getThreshold() { return *thresholdParam; }
    int getGateType() { return int(*gateTypeParam); }
    bool getOnState() { return bool(*onStateParam); }
    float getAttack() { return *attackParam; }
    float getRelease() { return *releaseParam; }
    float getHold() { return *holdParam; }
    void processBlock(juce::AudioBuffer<float>& buffer, int _totalNumInputChannels, int sampleRate);
private:
    juce::AudioProcessorValueTreeState& apvts;
    int totalNumInputChannels = 2;

    

    std::atomic<float>* thresholdParam;
    std::atomic<float>* gateTypeParam;
    std::atomic<float>* onStateParam;
    std::atomic<float>* attackParam;
    std::atomic<float>* releaseParam;
    std::atomic<float>* holdParam;

    void processGateSimple(juce::AudioBuffer<float>& buffer);
    void processGateMedium(juce::AudioBuffer<float>& buffer);
    void processGateAdvanced(juce::AudioBuffer<float>& buffer, int sampleRate);


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

