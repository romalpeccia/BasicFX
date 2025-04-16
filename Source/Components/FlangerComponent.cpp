/*
  ==============================================================================

    FlangerComponent.cpp
    Created: 13 Apr 2025 11:05:52pm
    Author:  romal

  ==============================================================================
*/

#include "FlangerComponent.h"

FlangerComponent::FlangerComponent(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    flangerSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_DELAY_STRING, flangerSlider);
    mixSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_MIX_STRING, mixSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, FLANGER_ON_STRING, button);
    button.setClickingTogglesState(true);

    for (auto* comp : getFlangerComps()) {
        addAndMakeVisible(comp);
    }
};
std::vector<juce::Component*> FlangerComponent::getFlangerComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&flangerSlider);
    comps.push_back(&mixSlider);
    comps.push_back(&button);
    return comps;
};
void FlangerComponent::resized() {
    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    flangerSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    mixSlider.setBounds(topBounds.withTrimmedRight(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
};