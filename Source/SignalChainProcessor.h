/*
  ==============================================================================

    SignalChainProcessor.h
    Created: 14 May 2025 1:50:03pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <Juceheader.h>
#include "Processors/SwappableProcessor.h"
#include "SwappableComponentManager.h"


class SignalChainProcessor {
    public:
        SignalChainProcessor(SwappableComponentManager* _swappableComponentManager);
        ~SignalChainProcessor();

        void prepareToPlay(double sampleRate, int samplesPerBlock, int totalNumInputChannels);

        void processBlock(juce::AudioBuffer<float>& buffer);
        void rebuildSignalChain();


    private:
        std::vector<SwappableProcessor*> signalChain;
        SwappableComponentManager* swappableComponentManager = nullptr;
        std::atomic<bool> rebuildFlag;

            //set to true:
            //rebuild.store(true);
            //change from true to false, return current value
           // bool returnValue = rebuild.exchange(true);
      //retreive value
       // bool returnValue rebuild.load();
   
};