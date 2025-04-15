/*
  ==============================================================================

    FlangerComponent.h
    Created: 13 Apr 2025 11:05:52pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"


class FlangerComponent : public SwappableComponent {

    public:
        FlangerComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getFlangerComps();
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;
        CustomTextButton flangerButton{ "Flanger" , FLANGER_COLOUR_1};
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> flangerButtonAttachment;
        CustomSlider flangerDelaySlider{"Delay", "ms", FLANGER_COLOUR_1, FLANGER_COLOUR_2};
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerDelaySliderAttachment;
        CustomSlider flangerMixSlider{ "Mix", "%", FLANGER_COLOUR_1, FLANGER_COLOUR_2};
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> flangerMixAttachment;
};
