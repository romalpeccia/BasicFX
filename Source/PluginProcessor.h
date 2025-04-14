/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Utilities.h"
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
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

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

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioProcessorValueTreeState apvts{ *this, nullptr, "Parameters", createParameterLayout() };

    //GATE members
    std::atomic<float>* thresholdParam;
    std::atomic<float>* gateStateParam; 
    std::atomic<float>* gateOnParam;
    std::atomic<float>* attackParam;
    std::atomic<float>* releaseParam;
    std::atomic<float>* holdParam;

    void processGateSimple(juce::AudioBuffer<float>& buffer);
    void processGateMedium(juce::AudioBuffer<float>& buffer);
    void processGateAdvanced(juce::AudioBuffer<float>& buffer);
    bool attackActive = false, holdActive = false, releaseActive = false;
    int attackSampleCounter = 0, releaseSampleCounter = 0, holdSampleCounter = 0; 
    int retriggerHoldSampleCounter = 0;
    float attackStartRatio = 1.f; //for when an attack is triggered during a release phase

    //DISTORTION members
    std::atomic<float>* distortionParam;
    std::atomic<float>* distortionOnParam; //TODO: why does this not work as a bool? why does it work as a float? weird
    std::atomic<float>* distortionStateParam;

    void processDistortionWaveRectifier(juce::AudioBuffer<float>& buffer);
    void processDistortionBitCrusher(juce::AudioBuffer<float>& buffer);
    void processDistortionSoftClipperCubic(juce::AudioBuffer<float>& buffer);
    void processDistortionSoftClipperArcTan(juce::AudioBuffer<float>& buffer);
    void processDistortionSlewLimiter(juce::AudioBuffer<float>& buffer);
    float lastSampleFromPrevBuffer[16] = { 0 };

    //FLANGER members
    std::atomic<float>* flangerDelayParam;
    std::atomic<float>* flangerOnParam;
    std::atomic<float>* flangerMixParam;
    void processFlanger(juce::AudioBuffer<float>& buffer);
    float delayBuffer[MAX_INPUT_CHANNELS][MAX_BUFFER_SIZE] = { 0 };


private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessor)
};
