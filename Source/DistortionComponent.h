/*
  ==============================================================================

    DistortionComponent.h
    Created: 13 Apr 2025 11:05:58pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "Utilities.h"

class DistortionComponent : public juce::Component {
    public:
        DistortionComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getDistortionComps();
        void resized() override;

    private:
        juce::AudioProcessorValueTreeState& apvts;
        juce::TextButton distortionButton{ "Distortion" };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> distortionButtonAttachment;
        juce::ComboBox distortionMenu;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionMenuAttachment;
        juce::Slider distortionSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionSliderAttachment;
};