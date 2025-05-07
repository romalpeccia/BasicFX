/*
  ==============================================================================

    EQComponent.cpp
    Created: 6 May 2025 9:43:17pm
    Author:  romal

  ==============================================================================
*/

#include "EQComponent.h"

EQComponent::EQComponent(juce::AudioProcessorValueTreeState& apvts, int index) : SwappableComponent(std::make_unique<EQProcessor>(apvts, index)), apvts(apvts) {

    button.setClickingTogglesState(true);
    menu.addItem("LOW PASS", 1);
    menu.addItem("HIGH PASS", 2);
    menu.addItem("BAND PASS", 3);
    menu.setSelectedId(1);

    for (auto* comp : getEQComps()) {
        addAndMakeVisible(comp);
    }
}
void EQComponent::setComponentAttachments(int index) {
    sliderAttachment = nullptr;
    buttonAttachment = nullptr;
    menuAttachment = nullptr;
    sliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(EQ_AMOUNT_STRING, index), slider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(EQ_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(EQ_TYPE_STRING, index), menu);

}


std::vector<juce::Component*> EQComponent::getEQComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&button);
    comps.push_back(&menu);
    comps.push_back(&slider);
    return comps;
}

void EQComponent::resized() {
    SwappableComponent::resized();

    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    slider.setBounds(bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    menu.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.6).withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));

    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
}