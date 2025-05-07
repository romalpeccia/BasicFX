/*
  ==============================================================================

    SwappableProcessor.h
    Created: 16 Apr 2025 12:55:18am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SwappableProcessor : public juce::ActionListener {
    public: 
        SwappableProcessor(int index) : processorIndex(index) {}
        ~SwappableProcessor() {};
        //pure virtual functions ( =0 ) are required for inheritance by subclasses 

        virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0; 
        virtual void prepareToPlay(double sampleRate, int _totalNumInputChannels) = 0; 
        virtual void assignParamPointers(int index) = 0;    //changes the pointers of the processor to point to the params cooresponding to the index supplied
        virtual void moveParamValues(int index) = 0;
        virtual void swapParamValues(SwappableProcessor* otherProcessor) = 0;
        void setProcessorIndex(int index) { processorIndex = index; }
        int getProcessorIndex() { return processorIndex; }

        void actionListenerCallback(const juce::String& message) override {};
    private:
            int processorIndex; //TODO: is this obsolete now?
};



class EmptyProcessor : public SwappableProcessor{
    //A processor that does nothing. For use in creating EmptyComponents while still maintaining extendablity of SwappableProcessor and SwappableComponent
    public:
        EmptyProcessor(int index) : SwappableProcessor(index) {}
        ~EmptyProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) override {}
        void prepareToPlay(double sampleRate, int _totalNumInputChannels) override {}
        void assignParamPointers(int index) override {}
        void moveParamValues(int index) override {}
        virtual void swapParamValues(SwappableProcessor* otherProcessor) override {};

};