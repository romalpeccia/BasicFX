/*
  ==============================================================================

    DistortionComponent.cpp
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionComponent.h"

DistortionComponent::DistortionComponent(juce::AudioProcessorValueTreeState& apvts, int index, SwappableComponentManager* manager)
    : SwappableComponent(std::make_unique<DistortionProcessor>(apvts, manager->getManagerIndex(),  index), manager),apvts(apvts) {


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
void DistortionComponent::setComponentAttachments(int index) {
    sliderAttachment = nullptr;
    buttonAttachment = nullptr;
    menuAttachment = nullptr;
    int bandIndex = getManager()->getManagerIndex();
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(DISTORTION_AMOUNT_STRING, bandIndex, index), slider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeMultibandParamID(DISTORTION_ON_STRING, bandIndex, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeMultibandParamID(DISTORTION_TYPE_STRING, bandIndex, index), menu);

}


std::vector<juce::Component*> DistortionComponent::getDistortionComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&button);
    comps.push_back(&menu);
    comps.push_back(&slider);
    return comps;
}
void DistortionComponent::resized() {
    SwappableComponent::resized();

    auto bounds = getLocalBounds();

    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    slider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    menu.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.6).withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));

    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);

}