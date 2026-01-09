/*
  ==============================================================================

    SwappableProcessor.h
    Created: 16 Apr 2025 12:55:18am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
class SignalChainProcessor; //forward declaration of SignalChainProcessor to avoid circular dependencies

class SwappableProcessor : public juce::ActionListener {
    public: 
        SwappableProcessor() {}
        ~SwappableProcessor() {};
        //pure virtual functions ( =0 ) are required for inheritance by subclasses 

        virtual void processBlock(juce::AudioBuffer<float>& buffer) = 0; 
        virtual void prepareToPlay(double sampleRate, int _totalNumInputChannels) = 0; 
        virtual void assignParamPointers(int index) = 0;    //changes the pointers of the processor to point to the params cooresponding to the index supplied
        virtual void moveParamValues(int index) = 0;
        virtual void swapParamValues(SwappableProcessor* otherProcessor) = 0;
        virtual void updateFilters() {  } //optional override if the inheriting class needs to update internal proccessor variables

        int getIndex();
        SignalChainProcessor* getSignalChainProcessor() const;
        void setSignalChainProcessor(SignalChainProcessor* signalChainProcessor);

        void actionListenerCallback(const juce::String& message) override {};
    private:
        SignalChainProcessor* signalChainProcessor = nullptr;
};



class EmptyProcessor : public SwappableProcessor{
    //A processor that does nothing. For use in creating EmptyComponents while still maintaining extendablity of SwappableProcessor and SwappableComponent
    public:
        EmptyProcessor()  {}
        ~EmptyProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) override {}
        void prepareToPlay(double sampleRate, int _totalNumInputChannels) override {}
        void assignParamPointers(int index) override {}
        void moveParamValues(int index) override {}
        virtual void swapParamValues(SwappableProcessor* otherProcessor) override {};

};