/*
  ==============================================================================

    MultiBandSignalChainProcessor.h
    Created: 14 May 2025 1:59:01pm
    Author:  romal

  ==============================================================================
*/


#pragma once
#include "Utilities.h"
#include "SignalChainProcessor.h"
#include "MultiBandSignalChainManager.h"


class MultiBandSignalChainProcessor {
    public:
        MultiBandSignalChainProcessor() {}
        MultiBandSignalChainProcessor(MultiBandSignalChainManager* signalChainComponents);
        ~MultiBandSignalChainProcessor() {}

        void prepareToPlay(double sampleRate, int samplesPerBlock, int totalNumInputChannels) {

            for (auto& signalChainProcessor : signalChains)
            {
                signalChainProcessor->prepareToPlay(sampleRate, samplesPerBlock, totalNumInputChannels);
            }
        }

        void processBlock(juce::AudioBuffer<float>& buffer) {
           
            for (auto& signalChainProcessor : signalChains)
            {
                signalChainProcessor->processBlock(buffer);

            }

            
        }
        void rebuildSignalChains() {
            for (auto& signalChainProcessor : signalChains)
            {
                signalChainProcessor->rebuildSignalChain();
            }
        }


    private:
        std::vector<std::unique_ptr<SignalChainProcessor>> signalChains;
};