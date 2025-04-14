/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "GateComponent.h"
#include "DistortionComponent.h"
#include "FlangerComponent.h"
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

    GateComponent gateComponent{ apvts };

    DistortionComponent distortionComponent{ apvts };

    FlangerComponent flangerComponent{ apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
