/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/GateComponent.h"
#include "Components/DistortionComponent.h"
#include "Components/FlangerComponent.h"
#include "Components/DBMeterComponent.h"
#include "Components/VisualizerComponent.h"
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

    DBMeterComponent incomingDBMeterComponent{ audioProcessor.dbMeterIncomingProcessor };
    VisualizerComponent visualizerComponent{ audioProcessor.visualizerIncomingProcessor, audioProcessor.visualizerOutgoingProcessor };
    GateComponent gateComponent{ apvts };
    DistortionComponent distortionComponent{ apvts };
    FlangerComponent flangerComponent{ apvts };
    DBMeterComponent outgoingDBMeterComponent{ audioProcessor.dbMeterOutgoingProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
