/*
  ==============================================================================

    DistortionProcessor.h
    Created: 13 Apr 2025 11:05:18pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "SwappableProcessor.h"
class DistortionProcessor : public SwappableProcessor {
public:
    //DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts, int index);
    DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex);
    ~DistortionProcessor() {}

    bool getOnState() { return bool(*onStateParam); }
    float getAmount() { return *amountParam; }
    float getDistortionType() { return *distortionTypeParam; }
    void setOnState(bool value);
    void setAmount(float value);
    void setDistortionType(int value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) override { sampleRate = _sampleRate, totalNumInputChannels = _totalNumInputChannels; }
    
    void assignParamPointers(int index) override;
    void moveParamValues(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;
private:
    juce::AudioProcessorValueTreeState& apvts;
    int totalNumInputChannels = 2;
    float sampleRate = 44100.f;

    std::atomic<float>* amountParam;
    std::atomic<float>* onStateParam; //TODO: why does this not work as a bool? why does it work as a float? weird
    std::atomic<float>* distortionTypeParam;

    void processDistortionWaveRectifier(juce::AudioBuffer<float>& buffer);
    void processDistortionBitCrusher(juce::AudioBuffer<float>& buffer);
    void processDistortionSoftClipperCubic(juce::AudioBuffer<float>& buffer);
    void processDistortionSoftClipperArcTan(juce::AudioBuffer<float>& buffer);
    void processDistortionSlewLimiter(juce::AudioBuffer<float>& buffer);
    float bitCrush(float sample, int bits);

    float lastSampleFromPrevBuffer[16] = { 0 };
};