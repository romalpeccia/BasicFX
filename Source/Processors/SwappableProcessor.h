/*
  ==============================================================================

    SwappableProcessor.h
    Created: 16 Apr 2025 12:55:18am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SwappableProcessor {
    public: 
        ~SwappableProcessor() {};
        virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0; 
        virtual void prepareToPlay(double sampleRate, int _totalNumInputChannels) = 0; 
        int processorIndex;

};

class EmptyProcessor : public SwappableProcessor{
    //A processor that does nothing. For use in creating uninitialized Components
    public:
        EmptyProcessor() {}
        ~EmptyProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) {}
        void prepareToPlay(double sampleRate, int _totalNumInputChannels) {}
};