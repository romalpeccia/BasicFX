/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
#include "Processors/GateProcessor.h"
#include "Processors/DistortionProcessor.h"
#include "Processors/FlangerProcessor.h"
#include "Processors/DBMeterProcessor.h"
#include "Processors/VisualizerProcessor.h"

//==============================================================================
/**
*/

class BasicFXAudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    BasicFXAudioProcessor();
    ~BasicFXAudioProcessor() override;

    //==============================================================================

    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif



    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;


    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    void updateSignalChainOrder(const std::vector<ProcessorType>& order);
    std::vector<SwappableProcessor*> signalChain;

    GateProcessor gateProcessor{ apvts };
    DistortionProcessor distortionProcessor{ apvts };
    FlangerProcessor flangerProcessor{ apvts };
    DBMeterProcessor dbMeterIncomingProcessor;
    DBMeterProcessor dbMeterOutgoingProcessor;
    VisualizerProcessor visualizerIncomingProcessor{ 44100 * SAMPLE_RATE_FACTOR };
    VisualizerProcessor visualizerOutgoingProcessor{ 44100 * SAMPLE_RATE_FACTOR };
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessor)
};

