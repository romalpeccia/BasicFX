/*
  ==============================================================================

    DistortionComponent.h
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "CustomSlider.h"
#include "../SwappableComponentManager.h"
#include "../Processors/DistortionProcessor.h"
class DistortionComponent : public SwappableComponent {
    public:
        //DistortionComponent(juce::AudioProcessorValueTreeState& _apvts, int index);
        DistortionComponent(juce::AudioProcessorValueTreeState& _apvts, int index, SwappableComponentManager* manager);
        std::vector<juce::Component*> getDistortionComps();
        void setComponentAttachments(int index) override;
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;

        const juce::Colour DISTORTION_COLOUR_1 = juce::Colours::hotpink;
        const juce::Colour DISTORTION_COLOUR_2 = juce::Colours::deepskyblue;

        CustomTextButton button{ "Distortion", DISTORTION_COLOUR_1 };
        CustomComboBox menu{ DISTORTION_COLOUR_1, DISTORTION_COLOUR_2 };
        CustomSlider slider{ "Amount", "", DISTORTION_COLOUR_1,  DISTORTION_COLOUR_2 };

        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> menuAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
};