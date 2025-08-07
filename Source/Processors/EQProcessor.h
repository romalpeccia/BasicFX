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
#include <juce_dsp/juce_dsp.h>
class EQProcessor : public SwappableProcessor{
public:
    //EQProcessor(juce::AudioProcessorValueTreeState& _apvts, int index);
    EQProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex);
    ~EQProcessor() {}

    float getLowFrequency() { return *lowFrequencyParam; }
    float getHighFrequency() { return *highFrequencyParam; }
    bool getOnState() { return bool(*onStateParam); }
    float getEQType() { return *eqTypeParam; }

    void setOnState(bool value);
    void setLowFrequency(float value);
    void setHighFrequency(float value);
    void setEQType(int value);

    void processBlock(juce::AudioBuffer<float>& buffer) override;
    void prepareToPlay(double _sampleRate, int _totalNumInputChannels) override;
    void assignParamPointers(int index) override;
    void moveParamValues(int index) override;
    void swapParamValues(SwappableProcessor* otherProcessor) override;

    void updateFilters();
    void actionListenerCallback(const juce::String& message) override;
private:
    juce::AudioProcessorValueTreeState& apvts;

    std::vector<juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>> highPassFilters;

    std::vector<juce::dsp::ProcessorDuplicator<
        juce::dsp::IIR::Filter<float>,
        juce::dsp::IIR::Coefficients<float>>> lowPassFilters;

    std::vector<juce::dsp::ProcessorChain<
        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>, // HPF
        juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>, juce::dsp::IIR::Coefficients<float>>  // LPF
        >> bandPassFilters;


    std::atomic<float>* lowFrequencyParam;
    std::atomic<float>* highFrequencyParam;
    std::atomic<float>* onStateParam; //TODO: why does this not work as a bool? why does it work as a float? weird
    std::atomic<float>* eqTypeParam;

    float sampleRate = 44100.0f;
    int totalNumInputChannels = 2;
};