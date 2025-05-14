/*
  ==============================================================================

    SignalChainProcessor.cpp
    Created: 14 May 2025 1:50:03pm
    Author:  romal

  ==============================================================================
*/

#include "SignalChainProcessor.h"

SignalChainProcessor::SignalChainProcessor(SwappableComponentManager* _swappableComponentManager) : swappableComponentManager(_swappableComponentManager) {
};
SignalChainProcessor::~SignalChainProcessor() {}

void SignalChainProcessor::prepareToPlay(double sampleRate, int samplesPerBlock, int totalNumInputChannels) {

    for (auto* processor : signalChain)
    {
        processor->prepareToPlay(sampleRate, totalNumInputChannels);
    }
}

void SignalChainProcessor::processBlock(juce::AudioBuffer<float>& buffer) {
    for (auto* processor : signalChain)
    {
        processor->processBlock(buffer);
    }
}
void SignalChainProcessor::rebuildSignalChain() {
    auto& componentList = swappableComponentManager->getComponentList();
    signalChain.clear();
    for (auto* comp : componentList)
    {
        if (comp->getProcessor() != nullptr)
            signalChain.push_back(comp->getProcessor());
    }
}