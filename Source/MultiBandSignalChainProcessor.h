
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
            //TODO: fix chain parralelization
            if (!signalChains.empty()) {
                int channels = buffer.getNumChannels();
                int numSamples = buffer.getNumSamples();
                //make MAX_BANDS copies of the buffer
                juce::AudioBuffer<float> bufferCopies[MAX_BANDS];
                for (int i = 0; i < MAX_BANDS; i++) {
                    bufferCopies[i].makeCopyOf(buffer);
                }

                //iterate through each signalChain and process each buffer
                for (int i = 0; i < signalChains.size(); ++i) {
                    signalChains[i]->processBlock(bufferCopies[i]);
                }

                //normalize each buffer
                for (int i = 0; i < MAX_BANDS; i++) {
                    for (int channel = 0; channel < channels; channel++) {
                        auto* channelData = bufferCopies[i].getWritePointer(channel);
                        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
                            channelData[sampleNum] *= (1.f / float(MAX_BANDS));
                        }
                    }
                }

                //aggregate the buffers 
                for (int i = 1; i < MAX_BANDS; i++) {
                    for (int j = 0; j < channels; j++) {
                        bufferCopies[0].addFrom(j, 0, bufferCopies[i], j, 0, numSamples, 1.f);
                    }
                }
                //copy to original buffer
                buffer.makeCopyOf(bufferCopies[0]);
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