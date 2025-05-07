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
    menu.addItem("BAND PASS", 2);
    menu.addItem("HIGH PASS", 3);
    menu.setSelectedId(1);

    addActionListener(processor.get());
    lowFrequencySlider.onValueChange = [this]() {
        sendActionMessage("");
        };
    highFrequencySlider.onValueChange = [this]() {
        sendActionMessage("");
        };
    menu.onChange = [this](){
        resized();
    };
    for (auto* comp : getEQComps()) {
        addAndMakeVisible(comp);
    }
}
void EQComponent::setComponentAttachments(int index) {
    lowFrequencySliderAttachment = nullptr;
    highFrequencySliderAttachment = nullptr;
    buttonAttachment = nullptr;
    menuAttachment = nullptr;
    lowFrequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(EQ_LOW_FREQUENCY_STRING, index), lowFrequencySlider);
    highFrequencySliderAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, makeID(EQ_HIGH_FREQUENCY_STRING, index), highFrequencySlider);
    buttonAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(apvts, makeID(EQ_ON_STRING, index), button);
    menuAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(apvts, makeID(EQ_TYPE_STRING, index), menu);

}


std::vector<juce::Component*> EQComponent::getEQComps() {
    std::vector<juce::Component*> comps;
    comps.push_back(&button);
    comps.push_back(&menu);
    comps.push_back(&lowFrequencySlider);
    comps.push_back(&highFrequencySlider);
    return comps;
}

void EQComponent::resized() {
    SwappableComponent::resized();

    auto bounds = getLocalBounds();
    auto bottomBounds = bounds.withTrimmedTop(bounds.getHeight() * 0.6);
    menu.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.6).withTrimmedBottom(bottomBounds.getHeight() * 0.2));
    button.setBounds(bottomBounds.withTrimmedTop(bottomBounds.getHeight() * 0.8));
    auto sliderBounds = bottomBounds.withTrimmedBottom(bottomBounds.getHeight() * 0.2);
    if (menu.getText() == "LOW PASS" || menu.getText() == "BAND PASS") {
        lowFrequencySlider.setVisible(true);
        lowFrequencySlider.setBounds(sliderBounds.withTrimmedRight(sliderBounds.getWidth() * 0.5));
    }
    else {
        lowFrequencySlider.setVisible(false);
    }
    if (menu.getText() == "HIGH PASS" || menu.getText() == "BAND PASS") {
        highFrequencySlider.setVisible(true);
        highFrequencySlider.setBounds(sliderBounds.withTrimmedLeft(sliderBounds.getWidth() * 0.5));
    }
    else {
        highFrequencySlider.setVisible(false);
    }
    auto topBounds = bounds.withTrimmedBottom(bounds.getHeight() * 0.4);
}