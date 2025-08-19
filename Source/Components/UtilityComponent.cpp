/*
  ==============================================================================

    UtilityComponent.cpp
    Created: 19 Aug 2025 5:26:58pm
    Author:  romal

  ==============================================================================
*/

#include "UtilityComponent.h"
#include "DistortionComponent.h"

UtilityComponent::UtilityComponent(juce::AudioProcessorValueTreeState& apvts, int index, SwappableComponentManager* manager)
    : SwappableComponent(std::make_unique<UtilityProcessor>(apvts, manager->getManagerIndex(), index), manager), apvts(apvts) {

    button.setClickingTogglesState(true);

    for (auto* comp : getUtilityComps()) {
        addAndMakeVisible(comp);
    }
}
void UtilityComponent::setComponentAttachments(int index) {
    volumeAttachment = nullptr;
    stereoAttachment = nullptr;
    buttonAttachment = nullptr;

    int bandIndex = getManager()->getManagerIndex();
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(UTILITY_VOLUME_STRING, bandIndex, index), volumeSlider);
    stereoAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeMultibandParamID(UTILITY_STEREO_STRING, bandIndex, index), stereoSlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeMultibandParamID(UTILITY_ON_STRING, bandIndex, index), button);
   
}


std::vector<juce::Component*> UtilityComponent::getUtilityComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&button);
    comps.push_back(&stereoSlider);
    comps.push_back(&volumeSlider);
    return comps;
}
void UtilityComponent::resized() {
    SwappableComponent::resized();

    auto bounds = getLocalBounds();

    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    
    volumeSlider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));

    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
    stereoSlider.setBounds(topBounds.withTrimmedBottom(topBounds.getHeight() * 0.2));

}