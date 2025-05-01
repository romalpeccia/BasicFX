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
#include "../Processors/GateProcessor.h"
#include "../SwappableComponentManager.h"
class GateComponent : public SwappableComponent {
    public:
        GateComponent(juce::AudioProcessorValueTreeState& apvts, int index);
        ~GateComponent() {}

        //initialization helpers
        std::vector<juce::Component*> getGateComps();
        void setComponentAttachments() override;

        void resized() override;
    private:
        const juce::Colour GATE_COLOUR_1 = juce::Colours::mediumpurple;
        const juce::Colour GATE_COLOUR_2 = juce::Colours::darkorange;

        juce::AudioProcessorValueTreeState& apvts;
        CustomComboBox menu{ GATE_COLOUR_1, GATE_COLOUR_2 };
        CustomTextButton button{ "Gate" ,GATE_COLOUR_1};
        CustomSlider thresholdSlider{"Threshold", "amplitude", GATE_COLOUR_1, GATE_COLOUR_2};
        CustomSlider attackSlider{"Attack", "s", GATE_COLOUR_1, GATE_COLOUR_2};
        CustomSlider holdSlider{"Hold", "s", GATE_COLOUR_1, GATE_COLOUR_2 };
        CustomSlider releaseSlider{"Release", "s", GATE_COLOUR_1, GATE_COLOUR_2 };
        std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> menuAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, attackAttachment, holdAttachment, releaseAttachment;

};

