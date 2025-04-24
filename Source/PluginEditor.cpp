/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicFXAudioProcessorEditor::BasicFXAudioProcessorEditor (BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState & _apvts)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(_apvts)
{

    for (auto* comp : getVisibleComps())
    {
        addAndMakeVisible(comp);
    }

    setSize (1200, 800);
    setResizable(true, true);
}

BasicFXAudioProcessorEditor::~BasicFXAudioProcessorEditor()
{
}


//==============================================================================
void BasicFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void BasicFXAudioProcessorEditor::resized()
{
    auto pluginBounds = getLocalBounds();

    auto FXBounds = pluginBounds.withTrimmedTop(pluginBounds.getHeight()*0.2);
    auto monitoringAreaBounds  = pluginBounds.withTrimmedBottom(pluginBounds.getHeight() * 0.8);

    auto visualizerBounds = monitoringAreaBounds.withTrimmedRight(monitoringAreaBounds.getWidth() * 0.1).withTrimmedLeft(monitoringAreaBounds.getWidth() * 0.1);
    auto incomingDBMeterBounds = monitoringAreaBounds.withTrimmedRight(monitoringAreaBounds.getWidth() * 0.9);
    auto outgoingDBMeterBounds = monitoringAreaBounds.withTrimmedLeft(monitoringAreaBounds.getWidth() * 0.9);
    visualizerComponent.setBounds(visualizerBounds);
    incomingDBMeterComponent.setBounds(incomingDBMeterBounds);
    outgoingDBMeterComponent.setBounds(outgoingDBMeterBounds);
    
    swappableComponentManager.setBounds(FXBounds);
}



std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&visualizerComponent);
    comps.push_back(&incomingDBMeterComponent);
    comps.push_back(&outgoingDBMeterComponent);
    comps.push_back(&swappableComponentManager);
    return comps;
}

