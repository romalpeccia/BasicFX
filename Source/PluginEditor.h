/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SignalChainComponent.h"
#include "Components/DBMeterComponent.h"
#include "Components/VisualizerComponent.h"

class BasicFXAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicFXAudioProcessorEditor (BasicFXAudioProcessor&, juce::AudioProcessorValueTreeState& _apvts);
    ~BasicFXAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

    std::vector<juce::Component*>  getVisibleComps();

private:

    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    //TODO decouple processor and component
    DBMeterComponent incomingDBMeterComponent{apvts, &audioProcessor.dbMeterIncomingProcessor };
    VisualizerComponent visualizerComponent{ &audioProcessor.visualizerIncomingProcessor, &audioProcessor.visualizerOutgoingProcessor };
    DBMeterComponent outgoingDBMeterComponent{apvts, &audioProcessor.dbMeterOutgoingProcessor };

    SignalChainComponent signalChainComponent{ audioProcessor, apvts };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
