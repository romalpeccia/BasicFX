/*
  ==============================================================================

    GateComponent.cpp
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#include "GateComponent.h"


GateComponent::GateComponent(juce::AudioProcessorValueTreeState& _apvts, GateProcessor* gateProcessor) : SwappableComponent(gateProcessor), apvts(_apvts) {

    int index = getIndexInComponentList();
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(THRESHOLD_STRING, index), thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(ATTACK_STRING, index), attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(RELEASE_STRING, index), releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(HOLD_STRING, index), holdSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(GATE_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(GATE_STATE_STRING, index), menu);
    
    button.setClickingTogglesState(true);
    menu.addItem(BASIC_GATE_STRING, 1); 
    menu.addItem(RMS_GATE_STRING, 2);
    menu.addItem(ATTACK_HOLD_RELEASE_GATE_STRING, 3);
    menu.setSelectedId(3);

    for (auto* comp : getGateComps()) {
        addAndMakeVisible(comp);
    }
}

GateComponent::GateComponent(juce::AudioProcessorValueTreeState& _apvts, GateProcessor* gateProcessor, int index) : SwappableComponent(gateProcessor, index), apvts(_apvts) {

    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(THRESHOLD_STRING, index), thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(ATTACK_STRING, index), attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(RELEASE_STRING, index), releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(HOLD_STRING, index), holdSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(GATE_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(GATE_STATE_STRING, index), menu);

    button.setClickingTogglesState(true);
    menu.addItem(BASIC_GATE_STRING, 1);
    menu.addItem(RMS_GATE_STRING, 2);
    menu.addItem(ATTACK_HOLD_RELEASE_GATE_STRING, 3);
    menu.setSelectedId(3);

    for (auto* comp : getGateComps()) {
        addAndMakeVisible(comp);
    }
}

void GateComponent::resized() {
    auto bounds = getLocalBounds();

    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    thresholdSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);

    menu.setBounds(topBounds.withTrimmedTop(topBounds.getHeight() * 0.9));
    attackSlider.setBounds(topBounds.withTrimmedRight(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
    holdSlider.setBounds(topBounds.withTrimmedLeft(topBounds.getWidth() * 0.33).withTrimmedRight(topBounds.getWidth() * 0.33).withTrimmedBottom(topBounds.getHeight() * 0.1));
    releaseSlider.setBounds(topBounds.withTrimmedLeft(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
}

std::vector<juce::Component*> GateComponent::getGateComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&thresholdSlider);
    comps.push_back(&menu);
    comps.push_back(&button);
    comps.push_back(&attackSlider);
    comps.push_back(&releaseSlider);
    comps.push_back(&holdSlider);
    return comps;
}