/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class BasicFXAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    BasicFXAudioProcessorEditor (BasicFXAudioProcessor&, juce::AudioProcessorValueTreeState& _apvts);
    ~BasicFXAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    std::vector<juce::Component*>  getVisibleComps();
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;


    //noise gate components
    juce::TextButton gateButton{ "Gate" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> gateButtonAttachment;
    juce::Slider thresholdSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow};
    juce::Slider attackSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider holdSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    juce::Slider releaseSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thresholdAttachment, attackAttachment, holdAttachment, releaseAttachment;
    juce::ComboBox gateMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> gateMenuAttachment;


    juce::TextButton distortionButton{ "Distortion" };
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> distortionButtonAttachment;
    juce::ComboBox distortionMenu;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> distortionMenuAttachment;
    juce::Slider distortionSlider{ juce::Slider::SliderStyle::Rotary,  juce::Slider::TextEntryBoxPosition::TextBoxBelow };
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> distortionSliderAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BasicFXAudioProcessorEditor)
};
