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
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto pluginBounds = getLocalBounds();

    auto FXBounds = pluginBounds.withTrimmedTop(pluginBounds.getHeight()*0.2);
    auto visualizerBounds = pluginBounds.withTrimmedBottom(pluginBounds.getHeight() * 0.8);

    auto incomingDBMeterBounds = visualizerBounds.withTrimmedRight(visualizerBounds.getWidth() * 0.8);
    incomingDBMeterComponent.setBounds(incomingDBMeterBounds);
    auto outgoingDBMeterBounds = visualizerBounds.withTrimmedLeft(visualizerBounds.getWidth() * 0.8);
    outgoingDBMeterComponent.setBounds(outgoingDBMeterBounds);

    auto gateBounds = FXBounds.withTrimmedRight(FXBounds.getWidth() * 0.75);
    auto distortionBounds = FXBounds.withTrimmedLeft(FXBounds.getWidth() * 0.25).withTrimmedRight(FXBounds.getWidth() * 0.5);
    auto flangerBounds = FXBounds.withTrimmedRight(FXBounds.getWidth() * 0.25).withTrimmedLeft(FXBounds.getWidth() * 0.5);
    auto bounds4 = FXBounds.withTrimmedLeft(FXBounds.getWidth() * 0.75);

    gateComponent.setBounds(gateBounds);
    distortionComponent.setBounds(distortionBounds);
    flangerComponent.setBounds(flangerBounds);
}

std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&gateComponent);
    comps.push_back(&distortionComponent);
    comps.push_back(&flangerComponent);
    comps.push_back(&incomingDBMeterComponent);
    comps.push_back(&outgoingDBMeterComponent);
    return comps;
}