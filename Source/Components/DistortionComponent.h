/*
  ==============================================================================

    DistortionComponent.h
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"

class DistortionComponent : public juce::Component {
    public:
        DistortionComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getDistortionComps();
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;


        CustomTextButton distortionButton{ "Distortion", DISTORTION_COLOUR_1 };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> distortionButtonAttachment;
        CustomComboBox distortionMenu{DISTORTION_COLOUR_1, DISTORTION_COLOUR_2};
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionMenuAttachment;
        CustomSlider distortionSlider{ "Amount", "", DISTORTION_COLOUR_1,  DISTORTION_COLOUR_2};
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionSliderAttachment;
};