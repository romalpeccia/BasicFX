/*
  ==============================================================================

    FlangerComponent.h
    Created: 13 Apr 2025 11:05:52pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "CustomSlider.h"

class FlangerComponent : public SwappableComponent {

    public:
        FlangerComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getFlangerComps();
        void resized() override;

    private:
        const juce::Colour FLANGER_COLOUR_1 = juce::Colours::darkolivegreen;
        const juce::Colour FLANGER_COLOUR_2 = juce::Colours::yellow;
        juce::AudioProcessorValueTreeState& apvts;
        CustomTextButton button{ "Flanger" , FLANGER_COLOUR_1};
        CustomSlider flangerSlider{"Delay", "ms", FLANGER_COLOUR_1, FLANGER_COLOUR_2};
        CustomSlider mixSlider{ "Mix", "%", FLANGER_COLOUR_1, FLANGER_COLOUR_2};
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerSliderAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> mixSliderAttachment;
};
