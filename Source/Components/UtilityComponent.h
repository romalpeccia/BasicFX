/*
  ==============================================================================

    UtilityComponent.h
    Created: 19 Aug 2025 5:26:58pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "CustomSlider.h"
#include "../SwappableComponentManager.h"
#include "../Processors/UtilityProcessor.h"
class UtilityComponent : public SwappableComponent {
public:
    UtilityComponent(juce::AudioProcessorValueTreeState& _apvts, int index, SwappableComponentManager* manager);
    std::vector<juce::Component*> getUtilityComps(); // TODO: make this a virtual function?
    void setComponentAttachments(int index) override;
    void resized() override;

private:
    juce::AudioProcessorValueTreeState& apvts;

    const juce::Colour UTILITY_COLOUR_1 = juce::Colours::darkslategrey;
    const juce::Colour UTILITY_COLOUR_2 = juce::Colours::lightslategrey;

    CustomTextButton button{ "Utility", UTILITY_COLOUR_1 };
    CustomSlider volumeSlider{ "Volume", "db", UTILITY_COLOUR_1,  UTILITY_COLOUR_2 };
    CustomSlider stereoSlider{ "Stereo", "L/R", UTILITY_COLOUR_1,  UTILITY_COLOUR_2 };

    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> buttonAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> stereoAttachment;
};