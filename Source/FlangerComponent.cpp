/*
  ==============================================================================

    FlangerComponent.cpp
    Created: 13 Apr 2025 11:05:52pm
    Author:  romal

  ==============================================================================
*/

#include "FlangerComponent.h"

FlangerComponent::FlangerComponent(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    flangerDelaySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_DELAY_STRING, flangerDelaySlider);
    flangerMixAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, FLANGER_MIX_STRING, flangerMixSlider);
    flangerButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, FLANGER_ON_STRING, flangerButton);
    flangerButton.setClickingTogglesState(true);
};
std::vector<juce::Component*> FlangerComponent::getFlangerComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&flangerDelaySlider);
    comps.push_back(&flangerMixSlider);
    comps.push_back(&flangerButton);
    return comps;
};
void FlangerComponent::resized() {
    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    flangerDelaySlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    flangerButton.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    flangerMixSlider.setBounds(topBounds.withTrimmedRight(topBounds.getWidth() * 0.66).withTrimmedBottom(topBounds.getHeight() * 0.1));
};