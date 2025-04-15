/*
  ==============================================================================

    GateComponent.h
    Created: 13 Apr 2025 11:05:47pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "CustomSlider.h"
class GateComponent : public SwappableComponent {
    public:
        GateComponent(juce::AudioProcessorValueTreeState& _apvts);
        std::vector<juce::Component*> getGateComps();
        void resized() override;
    private:
        juce::AudioProcessorValueTreeState& apvts;
        CustomTextButton gateButton{ "Gate" ,GATE_COLOUR_1};
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateButtonAttachment;
        CustomSlider thresholdSlider{"Threshold", "db", GATE_COLOUR_1, GATE_COLOUR_2};
        CustomSlider attackSlider{"Attack", "ms", GATE_COLOUR_1, GATE_COLOUR_2};
        CustomSlider holdSlider{"Hold", "ms", GATE_COLOUR_1, GATE_COLOUR_2 };
        CustomSlider releaseSlider{"Release", "ms", GATE_COLOUR_1, GATE_COLOUR_2 };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, attackAttachment, holdAttachment, releaseAttachment;
        CustomComboBox gateMenu{ GATE_COLOUR_1, GATE_COLOUR_2 };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gateMenuAttachment;
};

