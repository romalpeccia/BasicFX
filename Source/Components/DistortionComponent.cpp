/*
  ==============================================================================

    DistortionComponent.cpp
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionComponent.h"

DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& _apvts) : SwappableComponent(ProcessorType::Distortion), apvts(_apvts) {

    menu.addItem(WAVE_RECTIFIER_STRING, 1); //TODO what is up with these IDs? setting ID to 0 causes a bug, but it doesnt make sense that the indexes im putting are 1-4 when the param returns 0-3
    menu.addItem(BIT_CRUSHER_STRING, 2);
    menu.addItem(SOFT_CLIPPER_CUBIC_STRING, 3);
    menu.addItem(SOFT_CLIPPER_ARCTAN_STRING, 4);
    menu.addItem(SLEW_LIMITER_STRING, 5);
    menu.setSelectedId(1);

    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, DISTORTION_ON_STRING, button);
    button.setClickingTogglesState(true);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, DISTORTION_TYPE_STRING, menu);
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, DISTORTION_AMOUNT_STRING, slider);

    for (auto* comp : getDistortionComps()) {
        addAndMakeVisible(comp);
    }
}
std::vector<juce::Component*> DistortionComponent::getDistortionComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&button);
    comps.push_back(&menu);
    comps.push_back(&slider);
    return comps;
}
void DistortionComponent::resized() {
    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    slider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    menu.setBounds(topBounds.withTrimmedTop(topBounds.getHeight() * 0.9));
}