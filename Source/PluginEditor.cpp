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
    for (auto* comp : getSwappableComps())
    {
        comp->addChangeListener(&p);
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
    
    resizeSwappableComponentVector(FXBounds);
}

void BasicFXAudioProcessorEditor::resizeSwappableComponentVector(juce::Rectangle<int> bounds) {
    auto componentList = SwappableComponent::getSwappableComponents();
    if (!componentList.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() / componentList.size();
        float height = bounds.getHeight();
        //start at the corner of bounds and iterate. 
        for (auto* comp : componentList) {
            comp->setBounds(x, y, width, height);
            x += width;
        }
    }
}

std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&gateComponent);
    comps.push_back(&distortionComponent);
    comps.push_back(&flangerComponent);
    comps.push_back(&visualizerComponent);
    comps.push_back(&incomingDBMeterComponent);
    comps.push_back(&outgoingDBMeterComponent);
    return comps;
}

std::vector<SwappableComponent*> BasicFXAudioProcessorEditor::getSwappableComps() {
    std::vector<SwappableComponent*> comps;

    comps.push_back(&gateComponent);
    comps.push_back(&distortionComponent);
    comps.push_back(&flangerComponent);
    return comps;
}