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
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    DBMeterComponent incomingDBMeterComponent{ audioProcessor.incomingDBMeterProcessor };
    GateComponent gateComponent{ apvts };
    DistortionComponent distortionComponent{ apvts };
    FlangerComponent flangerComponent{ apvts };
    DBMeterComponent outgoingDBMeterComponent{ audioProcessor.outgoingDBMeterProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
