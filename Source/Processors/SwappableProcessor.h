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
        SwappableProcessor(int index) : paramIndex(index) {}
        ~SwappableProcessor() {};
        virtual void assignParamPointers(int index) = 0;
        virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0; 
        virtual void prepareToPlay(double sampleRate, int _totalNumInputChannels) = 0; 
        void setParamIndex(int index) { paramIndex = index; }
    private:
            int paramIndex;
};

class EmptyProcessor : public SwappableProcessor{
    //A processor that does nothing. For use in creating EmptyComponents while still maintaining extendablity of SwappableProcessor and SwappableComponent
    public:
        EmptyProcessor(int index) : SwappableProcessor(index) {}
        ~EmptyProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) {}
        void prepareToPlay(double sampleRate, int _totalNumInputChannels) {}
        void assignParamPointers(int index) override {}
};