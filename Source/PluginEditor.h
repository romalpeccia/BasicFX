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
class BasicFXAudioProcessorEditor  : public juce::AudioProcessorEditor
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

    std::vector<std::unique_ptr<SwappableComponent>> swappableComponents;

    DBMeterComponent incomingDBMeterComponent{apvts, &audioProcessor.dbMeterIncomingProcessor };
    VisualizerComponent visualizerComponent{ &audioProcessor.visualizerIncomingProcessor, &audioProcessor.visualizerOutgoingProcessor };
    /*
    GateComponent gateComponent{ apvts,  audioProcessor.gateProcessors[0].get()}; //note: why am i not passing the processor reference? because of the unique pointer?
    DistortionComponent distortionComponent{ apvts, audioProcessor.distortionProcessors[0].get() };
    DistortionComponent distortionComponent2{ apvts, audioProcessor.distortionProcessors[1].get() };
    FlangerComponent flangerComponent{ apvts, audioProcessor.flangerProcessors[0].get() };
    */

    DBMeterComponent outgoingDBMeterComponent{apvts, &audioProcessor.dbMeterOutgoingProcessor };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
