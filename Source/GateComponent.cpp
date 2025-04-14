/*
  ==============================================================================

    GateComponent.cpp
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#include "GateComponent.h"


GateComponent::GateComponent(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    gateButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, GATE_ON_STRING, gateButton);
    gateButton.setClickingTogglesState(true);
    gateMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, GATE_STATE_STRING, gateMenu);
    gateMenu.addItem(BASIC_GATE_STRING, 1); //TODO what is up with these IDs? setting ID to 0 causes a bug, but it doesnt make sense that the indexes im putting are 1-4 when the param returns 0-3
    gateMenu.addItem(RMS_GATE_STRING, 2);
    gateMenu.addItem(ATTACK_HOLD_RELEASE_GATE_STRING, 3);
    gateMenu.setSelectedId(1);
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, THRESHOLD_STRING, thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, ATTACK_STRING, attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, RELEASE_STRING, releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, HOLD_STRING, holdSlider);

    for (auto* comp : getGateComps()) {
        addAndMakeVisible(comp);
    }
}

void GateComponent::resized() {
    auto bounds = getLocalBounds();

    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    thresholdSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    gateButton.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);

    gateMenu.setBounds(topBounds.withTrimmedTop(topBounds.getHeight() * 0.9));
    attackSlider.setBounds(topBounds.withTrimmedRight(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
    holdSlider.setBounds(topBounds.withTrimmedLeft(topBounds.getWidth() * 0.33).withTrimmedRight(topBounds.getWidth() * 0.33).withTrimmedBottom(topBounds.getHeight() * 0.1));
    releaseSlider.setBounds(topBounds.withTrimmedLeft(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
}

std::vector<juce::Component*> GateComponent::getGateComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&thresholdSlider);
    comps.push_back(&gateMenu);
    comps.push_back(&gateButton);
    comps.push_back(&attackSlider);
    comps.push_back(&releaseSlider);
    comps.push_back(&holdSlider);
    return comps;
}