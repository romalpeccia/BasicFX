/*
  ==============================================================================

    GateComponent.cpp
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#include "GateComponent.h"



GateComponent::GateComponent(juce::AudioProcessorValueTreeState& apvts, int index, SwappableComponentManager* manager) 
    : SwappableComponent(std::make_unique<GateProcessor>(apvts, manager->getManagerIndex(), index), manager), apvts(apvts) {

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
    int bandIndex = getManager()->getManagerIndex();
    thresholdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(THRESHOLD_STRING, bandIndex, index), thresholdSlider);
    attackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(ATTACK_STRING, bandIndex, index), attackSlider);
    releaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(RELEASE_STRING, bandIndex, index), releaseSlider);
    holdAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(HOLD_STRING, bandIndex, index), holdSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeMultibandParamID(GATE_ON_STRING, bandIndex, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeMultibandParamID(GATE_STATE_STRING, bandIndex, index), menu);
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