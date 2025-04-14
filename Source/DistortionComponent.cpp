/*
  ==============================================================================

    DistortionComponent.cpp
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionComponent.h"

DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    distortionButtonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, DISTORTION_ON_STRING, distortionButton);
    distortionButton.setClickingTogglesState(true);
    distortionMenuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, DISTORTION_TYPE_STRING, distortionMenu);
    distortionMenu.addItem(WAVE_RECTIFIER_STRING, 1); //TODO what is up with these IDs? setting ID to 0 causes a bug, but it doesnt make sense that the indexes im putting are 1-4 when the param returns 0-3
    distortionMenu.addItem(BIT_CRUSHER_STRING, 2);
    distortionMenu.addItem(SOFT_CLIPPER_CUBIC_STRING, 3);
    distortionMenu.addItem(SOFT_CLIPPER_ARCTAN_STRING, 4);
    distortionMenu.addItem(SLEW_LIMITER_STRING, 5);
    distortionMenu.setSelectedId(1);
    distortionSliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DISTORTION_AMOUNT_STRING, distortionSlider);
}
std::vector<juce::Component*> DistortionComponent::getDistortionComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&distortionButton);
    comps.push_back(&distortionMenu);
    comps.push_back(&distortionSlider);
    return comps;
}
void DistortionComponent::resized() {
    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    distortionSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    distortionButton.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    distortionMenu.setBounds(topBounds.withTrimmedTop(topBounds.getHeight() * 0.9));
}