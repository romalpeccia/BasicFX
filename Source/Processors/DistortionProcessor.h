/*
  ==============================================================================

    DistortionProcessor.h
    Created: 13 Apr 2025 11:05:18pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"

class DistortionProcessor {
public:
    DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts);
    ~DistortionProcessor() {}
    float getAmount() { return *amountParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getGateType() { return *distortionTypeParam; }
    void processBlock(juce::AudioBuffer<float>& buffer);
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) { sampleRate = _sampleRate, totalNumInputChannels = _totalNumInputChannels; }
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
    float lastSampleFromPrevBuffer[16] = { 0 };
};