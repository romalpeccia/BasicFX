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
    void processBlock(juce::AudioBuffer<float>& buffer) {
        for (auto& processor : signalChain)
        {
            if (processor)
            processor->processBlock(buffer);
        }
    };
    void prepareToPlay(double sampleRate, int samplesPerBlock) {
        for (auto& processor : signalChain)
        {
            if (processor)
            processor->prepareToPlay(sampleRate, samplesPerBlock);
        }
    };


    void swapProcessorParams(int draggedIndex, int otherIndex);
    int getProcessorIndex(const SwappableProcessor &processor) {
        //returns index of a component if it is in std::vector<std::unique_ptr<SwappableProcessor>> signalChain
        for (int i = 0; i < signalChain.size(); i++)
            if (signalChain[i].get() == &processor)
                return i;
        return -1;
        
    }
    void actionListenerCallback(const juce::String& message);
private:
    std::vector<std::unique_ptr<SwappableProcessor>> signalChain;
    juce::AudioProcessorValueTreeState& apvts;
};