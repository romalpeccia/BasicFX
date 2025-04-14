/*
  ==============================================================================

    FlangerComponent.h
    Created: 13 Apr 2025 11:05:52pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "Utilities.h"
class FlangerComponent : public juce::Component {

    public:
        FlangerComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getFlangerComps();
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;
        juce::TextButton flangerButton{ "Flanger" };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerButtonAttachment;
        juce::Slider flangerDelaySlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDelaySliderAttachment;
        juce::Slider flangerMixSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerMixAttachment;
};