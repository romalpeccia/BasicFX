/*
  ==============================================================================

    GateComponent.cpp
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#include "GateComponent.h"



GateComponent::GateComponent(juce::AudioProcessorValueTreeState& apvts, int index) : SwappableComponent(std::make_unique<GateProcessor>(apvts, index)), apvts(apvts) {

    setComponentAttachments(index);
    button.setClickingTogglesState(true);
    menu.addItem(BASIC_GATE_STRING, 1);
    menu.addItem(RMS_GATE_STRING, 2);
    menu.addItem(ATTACK_HOLD_RELEASE_GATE_STRING, 3);
    menu.setSelectedId(3);

    for (auto* comp : getGateComps()) {
        addAndMakeVisible(comp);
    }
}

void GateComponent::setComponentAttachments(int index) {
    thresholdAttachment = nullptr;
    attackAttachment = nullptr;
    releaseAttachment = nullptr;
    holdAttachment = nullptr;
    buttonAttachment = nullptr;
    menuAttachment = nullptr;
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(THRESHOLD_STRING, index), thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(ATTACK_STRING, index), attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(RELEASE_STRING, index), releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(HOLD_STRING, index), holdSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(GATE_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(GATE_STATE_STRING, index), menu);
}

void GateComponent::resized() {
    SwappableComponent::resized();

    auto bounds = getLocalBounds();

    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    thresholdSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    menu.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.6).withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));

    auto sliderBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    attackSlider.setBounds(sliderBounds.withTrimmedBottom(sliderBounds.getHeight() * 0.66));
    holdSlider.setBounds(sliderBounds.withTrimmedBottom(sliderBounds.getHeight() * 0.33).withTrimmedTop(sliderBounds.getHeight() * 0.33));
    releaseSlider.setBounds(sliderBounds.withTrimmedTop(sliderBounds.getHeight() * 0.66));
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