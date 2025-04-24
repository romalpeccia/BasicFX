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
#include "Components/EmptyComponent.h"
//==============================================================================
/**
*/
class BasicFXAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ActionListener
{
public:
    BasicFXAudioProcessorEditor (BasicFXAudioProcessor&, juce::AudioProcessorValueTreeState& _apvts);
    ~BasicFXAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    std::vector<juce::Component*>  getVisibleComps();

    void resizeSwappableComponentVector(juce::Rectangle<int> bounds);

private:

    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
    void actionListenerCallback(const juce::String& message) override;

    std::vector<std::unique_ptr<SwappableComponent>> swappableComponents; // contains all of our components

    DBMeterComponent incomingDBMeterComponent{apvts, &audioProcessor.dbMeterIncomingProcessor };
    VisualizerComponent visualizerComponent{ &audioProcessor.visualizerIncomingProcessor, &audioProcessor.visualizerOutgoingProcessor };

    DBMeterComponent outgoingDBMeterComponent{apvts, &audioProcessor.dbMeterOutgoingProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
