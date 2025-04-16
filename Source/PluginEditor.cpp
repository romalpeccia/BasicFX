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
    SwappableComponent::setEditorReference(this);
    updateProcessorChainFromUI();
    for (auto* comp : getVisibleComps())
    {
        addAndMakeVisible(comp);
    }
    setSize (1200, 800);
}

BasicFXAudioProcessorEditor::~BasicFXAudioProcessorEditor()
{
}

void BasicFXAudioProcessorEditor::updateProcessorChainFromUI() {
    std::vector<ProcessorType> newOrder;
    for (auto* comp : SwappableComponent::getSwappableComponents()) {
        newOrder.push_back(comp->getProcessorType());
    }
    DBG("SwappableComponent count: " << SwappableComponent::getSwappableComponents().size());
    audioProcessor.updateSignalChainOrder(newOrder);
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
    auto incomingDBMeterBounds = monitoringAreaBounds .withTrimmedRight(monitoringAreaBounds .getWidth() * 0.9);
    auto outgoingDBMeterBounds = monitoringAreaBounds.withTrimmedLeft(monitoringAreaBounds.getWidth() * 0.9);
    visualizerComponent.setBounds(visualizerBounds);
    incomingDBMeterComponent.setBounds(incomingDBMeterBounds);
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
    comps.push_back(&visualizerComponent);
    comps.push_back(&incomingDBMeterComponent);
    comps.push_back(&outgoingDBMeterComponent);
    return comps;
}