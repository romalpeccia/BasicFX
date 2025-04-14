/*
  ==============================================================================

    GateComponent.h
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "Utilities.h"
class GateComponent : public juce::Component {
    public:
        GateComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getGateComps();
        void resized() override;
    private:
        juce::AudioProcessorValueTreeState& apvts;
        juce::TextButton gateButton{ "Gate" };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateButtonAttachment;
        juce::Slider thresholdSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        juce::Slider attackSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        juce::Slider holdSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        juce::Slider releaseSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, attackAttachment, holdAttachment, releaseAttachment;
        juce::ComboBox gateMenu;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gateMenuAttachment;
};

