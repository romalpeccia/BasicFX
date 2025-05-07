/*
  ==============================================================================

    EQComponent.h
    Created: 6 May 2025 9:43:17pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "CustomSlider.h"
#include "../SwappableComponentManager.h"
#include "../Processors/EQProcessor.h"

class EQComponent : public SwappableComponent {
public:
    EQComponent(juce::AudioProcessorValueTreeState& _apvts, int index);
    std::vector<juce::Component*> getEQComps();
    void setComponentAttachments(int index) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    const juce::Colour EQ_COLOUR_1 = juce::Colours::purple;
    const juce::Colour EQ_COLOUR_2 = juce::Colours::white;

    CustomTextButton button{ "EQ", EQ_COLOUR_1 };
    CustomComboBox menu{ EQ_COLOUR_1, EQ_COLOUR_2 };
    CustomSlider slider{ "Amount", "", EQ_COLOUR_1,  EQ_COLOUR_2 };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> menuAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sliderAttachment;
};