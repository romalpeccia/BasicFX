/*
  ==============================================================================

    SignalChainProcessor.h
    Created: 9 Oct 2025 1:45:47pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "Processors/SwappableProcessor.h"
#include "Processors/GateProcessor.h"
#include "Processors/DistortionProcessor.h"
#include "Processors/FlangerProcessor.h"
#include "Processors/EQProcessor.h"
#include "Utilities.h"


class SignalChainProcessor : public juce::ActionListener {

public:
    SignalChainProcessor(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
        signalChain.resize(MAX_COMPONENTS);
    }
    void processBlock(juce::AudioBuffer<float>&) {};
    void prepareToPlay(double sampleRate, int samplesPerBlock) {};

    void swapProcessorParams(int draggedIndex, int otherIndex);
    SwappableProcessor* getProcessor(int index) {}
    void actionListenerCallback(const juce::String& message);
private:
    std::vector<std::unique_ptr<SwappableProcessor>> signalChain;
    juce::AudioProcessorValueTreeState& apvts;
};