/*
  ==============================================================================

    DistortionComponent.cpp
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionComponent.h"

DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& apvts, int index): SwappableComponent(std::make_unique<DistortionProcessor>(apvts, index)),apvts(apvts) {


    button.setClickingTogglesState(true);
    menu.addItem(WAVE_RECTIFIER_STRING, 1);
    menu.addItem(BIT_CRUSHER_STRING, 2);
    menu.addItem(SOFT_CLIPPER_CUBIC_STRING, 3);
    menu.addItem(SOFT_CLIPPER_ARCTAN_STRING, 4);
    menu.addItem(SLEW_LIMITER_STRING, 5);
    menu.setSelectedId(1);

    for (auto* comp : getDistortionComps()) {
        addAndMakeVisible(comp);
    }
}
void DistortionComponent::setComponentAttachments() {
    int index = getManager()->findComponentIndex(*this);
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(DISTORTION_AMOUNT_STRING, index), slider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(DISTORTION_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(DISTORTION_TYPE_STRING, index), menu);
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