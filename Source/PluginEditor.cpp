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
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    gateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, GATE_STRING, gateSlider);

    for (auto* comp : getVisibleComps())
    {
        addAndMakeVisible(comp);
    }
    setSize (400, 300);
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
    auto bounds1 = pluginBounds.withTrimmedRight(pluginBounds.getWidth() * 0.75);
    auto bounds2 = pluginBounds.withTrimmedLeft(pluginBounds.getWidth() * 0.25).withTrimmedRight(pluginBounds.getWidth() * 0.5);
    auto bounds3 = pluginBounds.withTrimmedRight(pluginBounds.getWidth() * 0.25).withTrimmedLeft(pluginBounds.getWidth() * 0.5);
    auto bounds4 = pluginBounds.withTrimmedLeft(pluginBounds.getWidth() * 0.75);

    gateSlider.setBounds(bounds1);
}

std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&gateSlider);

    return comps;
}