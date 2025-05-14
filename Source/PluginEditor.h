/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SwappableComponentManager.h"
#include "Components/DBMeterComponent.h"
#include "Components/VisualizerComponent.h"
#include "MultiBandSignalChainManager.h"
//==============================================================================
/**
*/
class BasicFXAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicFXAudioProcessorEditor (BasicFXAudioProcessor&, juce::AudioProcessorValueTreeState& _apvts);
    ~BasicFXAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    std::vector<juce::Component*>  getVisibleComps();

private:

    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    DBMeterComponent incomingDBMeterComponent{apvts, &audioProcessor.dbMeterIncomingProcessor };
    VisualizerComponent visualizerComponent{ &audioProcessor.visualizerIncomingProcessor, &audioProcessor.visualizerOutgoingProcessor };
    DBMeterComponent outgoingDBMeterComponent{apvts, &audioProcessor.dbMeterOutgoingProcessor };
    MultiBandSignalChainManager MultiBandSignalChainManager{audioProcessor,  apvts};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
