/*
  ==============================================================================

    Utilities.h
    Created: 7 Apr 2025 5:06:31pm
    Author:  romal

  
  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Components/SwappableComponent.h"
using namespace juce;


const int MAX_INPUT_CHANNELS = 16;
const int REFRESH_RATE_MS = 10;

const juce::String GATE_STATE_STRING = "GATE_STATE";
const juce::String GATE_ON_STRING = "GATE_ON";
const juce::String THRESHOLD_STRING = "THRESHOLD";
const juce::String ATTACK_STRING = "ATTACK";
const juce::String HOLD_STRING = "HOLD";
const juce::String RELEASE_STRING = "RELEASE";
const juce::String BASIC_GATE_STRING = "BASIC";
const juce::String RMS_GATE_STRING = "RMS";
const juce::String ATTACK_HOLD_RELEASE_GATE_STRING = "ATTACK/HOLD/RELEASE";
const float ATTACK_MIN = 0.0001;
const float RELEASE_MIN = 0.0001;
const float HOLD_MIN = 0.0001;


const juce::String DISTORTION_AMOUNT_STRING = "DISTORTION_AMOUNT";
const juce::String DISTORTION_ON_STRING = "DISTORTION_ON";
const juce::String DISTORTION_TYPE_STRING = "DISTORTION_TYPE";
const juce::String WAVE_RECTIFIER_STRING = "WAVE_RECTIFIER";
const juce::String BIT_CRUSHER_STRING = "BIT_CRUSHER";
const juce::String SOFT_CLIPPER_CUBIC_STRING = "SOFT_CLIPPER_CUBIC";
const juce::String SOFT_CLIPPER_ARCTAN_STRING = "SOFT_CLIPPER_ARCTAN";
const juce::String SLEW_LIMITER_STRING = "SLEW_LIMITER";

const juce::String FLANGER_DELAY_STRING = "FLANGER_DELAY_MS";
const juce::String FLANGER_MIX_STRING = "FLANGER_MIX";
const juce::String FLANGER_ON_STRING = "FLANGER_ON";
const float DELAY_MAX = 1; 

const juce::Colour GATE_COLOUR_1 = juce::Colours::mediumpurple;
const juce::Colour GATE_COLOUR_2 = juce::Colours::darkorange;
const juce::Colour DISTORTION_COLOUR_1 = juce::Colours::hotpink;
const juce::Colour DISTORTION_COLOUR_2 = juce::Colours::deepskyblue;
const juce::Colour FLANGER_COLOUR_1 = juce::Colours::darkolivegreen;
const juce::Colour FLANGER_COLOUR_2 = juce::Colours::yellow;
const juce::Colour COMPONENT_COLOUR_OFF = juce::Colours::dimgrey;

float calculateRMS(const float* samples, int numSamples);
float calculateRMSAcrossChannels(juce::AudioBuffer<float>& buffer);
float calculateAverageAcrossChannels(const juce::AudioBuffer<float>& buffer); 

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override;
};


class CustomTextButton : public juce::TextButton {
    public:
        CustomTextButton() {
            setLookAndFeel(&lnf);
        };
        CustomTextButton(const juce::String& name, const juce::Colour buttonColour) {
            setLookAndFeel(&lnf);
            setColour(juce::TextButton::ColourIds::buttonOnColourId, buttonColour);
            setColour(juce::TextButton::ColourIds::buttonColourId, COMPONENT_COLOUR_OFF);
            setButtonText(name);
        };
        ~CustomTextButton() {
            setLookAndFeel(nullptr);
        };
        void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
        {
            lnf.drawButtonBackground(g, *this,
                findColour(getToggleState() ? juce::TextButton::ColourIds::buttonOnColourId : juce::TextButton::ColourIds::buttonColourId),
                shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
            lnf.drawButtonText(g, *this, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
        }

    private:
        CustomLookAndFeel lnf;
};


class CustomComboBox : public juce::ComboBox {
    public:
        CustomComboBox() {  }
        CustomComboBox(const juce::Colour mainColour, const juce::Colour secondaryColour) {
            setColour(juce::ComboBox::ColourIds::outlineColourId, mainColour);
            setColour(juce::ComboBox::ColourIds::textColourId, secondaryColour.brighter(0.5f));
            setColour(juce::ComboBox::ColourIds::backgroundColourId, mainColour.darker(0.5f));
            //TODO: some of these dont apply to the dropDown menu as expected
            setColour(juce::ComboBox::ColourIds::arrowColourId, mainColour);
            setColour(juce::ComboBox::ColourIds::focusedOutlineColourId, secondaryColour);
            setColour(juce::ComboBox::ColourIds::buttonColourId, secondaryColour);
            //setColour(juce::ComboBox::ColourIds::backgroundColourId, COMPONENT_COLOUR_OFF);
            //setLookAndFeel(&lnf);
        };
        ~CustomComboBox() {}
    private:
        //CustomLookAndFeel lnf;
};

