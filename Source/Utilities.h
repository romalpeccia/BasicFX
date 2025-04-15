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
const int MAX_BUFFER_SIZE = 9999; // remove this once i come up with a better buffer

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
//const float DELAY_MAX = 0.01; 
const float DELAY_MAX = 1; //for debugging

const juce::Colour GATE_COLOUR_1 = juce::Colours::mediumpurple;
const juce::Colour GATE_COLOUR_2 = juce::Colours::darkorange;
const juce::Colour DISTORTION_COLOUR_1 = juce::Colours::hotpink;
const juce::Colour DISTORTION_COLOUR_2 = juce::Colours::deepskyblue;
const juce::Colour FLANGER_COLOUR_1 = juce::Colours::darkolivegreen;
const juce::Colour FLANGER_COLOUR_2 = juce::Colours::yellow;
const juce::Colour COMPONENT_COLOUR_OFF = juce::Colours::dimgrey;

class CustomLookAndFeel : public juce::LookAndFeel_V4 {
public:

    juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override
    {
        juce::Slider::SliderLayout layout;

        // The bounds of the entire component
        auto bounds = slider.getLocalBounds();

        // You define where the slider part goes
        layout.sliderBounds = bounds; // or a custom area

        // Define your custom textbox bounds
        int textAreaWidth = bounds.getWidth()/2;
        int textAreaHeight = (bounds.getHeight() > textAreaWidth) ? textAreaWidth/2 : bounds.getHeight()/2;
        layout.textBoxBounds = juce::Rectangle<int>(
            bounds.getCentreX() - textAreaWidth/2 ,
            bounds.getCentreY() - textAreaHeight/2,
            textAreaWidth,
            textAreaHeight
        );

        return layout;
    }

    void drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider) override
    {
        auto outline = slider.findColour(Slider::rotarySliderOutlineColourId);
        auto fill = slider.findColour(Slider::rotarySliderFillColourId);

        auto bounds = Rectangle<int>(x, y, width, height).toFloat();

        auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        auto lineW = jmin(8.0f, radius * 0.5f);
        auto arcRadius = radius - lineW * 0.5f;

        Path backgroundArc;
        backgroundArc.addCentredArc(bounds.getCentreX(),
            bounds.getCentreY(),
            arcRadius,
            arcRadius,
            0.0f,
            rotaryStartAngle,
            rotaryEndAngle,
            true);

        g.setColour(outline);
        g.strokePath(backgroundArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));

        if (slider.isEnabled())
        {
            Path valueArc;
            valueArc.addCentredArc(bounds.getCentreX(),
                bounds.getCentreY(),
                arcRadius,
                arcRadius,
                0.0f,
                rotaryStartAngle,
                toAngle,
                true);

            g.setColour(fill);
            g.strokePath(valueArc, PathStrokeType(lineW, PathStrokeType::curved, PathStrokeType::rounded));
        }

        auto thumbWidth = lineW * 2.0f;
        Point<float> thumbPoint(bounds.getCentreX() + arcRadius * std::cos(toAngle - MathConstants<float>::halfPi),
            bounds.getCentreY() + arcRadius * std::sin(toAngle - MathConstants<float>::halfPi));
        g.setColour(slider.findColour(Slider::thumbColourId));
        g.fillEllipse(Rectangle<float>(thumbWidth, thumbWidth).withCentre(thumbPoint));
    }
};

class CustomSlider : public juce::Slider {

    public:
        CustomSlider(juce::String name) {
            setColour(juce::Slider::ColourIds::backgroundColourId, COMPONENT_COLOUR_OFF);
            setName(name);
            setLookAndFeel(&lnf);
        };
        CustomSlider(juce::String name, juce::String units, const juce::Colour mainColour, const juce::Colour secondaryColour) {
            setColour(juce::Slider::ColourIds::textBoxOutlineColourId, mainColour);
            setColour(juce::Slider::ColourIds::textBoxTextColourId, secondaryColour.brighter(0.5f));
            setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, mainColour.darker(0.5f));
            setColour(juce::Slider::ColourIds::trackColourId, mainColour);
            setColour(juce::Slider::ColourIds::thumbColourId, secondaryColour);
            setColour(juce::Slider::ColourIds::backgroundColourId, COMPONENT_COLOUR_OFF);
            setSliderStyle(juce::Slider::SliderStyle::Rotary);
            setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
            setName(name);
            setUnits(units);
            setLookAndFeel(&lnf);

        };
        ~CustomSlider() {
            setLookAndFeel(nullptr);
        };

        juce::String getUnits() { return units; }
        void setUnits(juce::String _units) {
            units = _units; updateText();
            
        }

        void paint(juce::Graphics& g) override {
            auto startAngle = juce::degreesToRadians(180.f + 45.f);
            auto endAngle = juce::degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi; 
            auto range = getRange();
            auto bounds = getLocalBounds();
            auto height = bounds.getHeight();
            auto width = bounds.getWidth();
            auto x = bounds.getX();
            auto y = bounds.getY();
            getLookAndFeel().drawRotarySlider(g, x, y, width , height ,jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAngle, endAngle, *this);

            //TODO: make this nicer looking / cleaner / make text editable
            int textAreaWidth = bounds.getWidth() / 2;
            int textAreaHeight = (bounds.getHeight() > textAreaWidth) ? textAreaWidth / 2 : bounds.getHeight() / 2;
            x = bounds.getCentre().getX() - textAreaWidth/2 - textAreaWidth / 10;
            y = bounds.getCentre().getY() - textAreaHeight/2;
            int fontHeight = textAreaHeight /2;
            fontHeight = (fontHeight > 5) ? fontHeight : 5;
            g.setFont(fontHeight);
            g.drawText(getName(), x, y , juce::Font(fontHeight).getStringWidth(getName()), fontHeight, juce::Justification::centred);
            g.drawText(juce::String(getValue()), x, y + fontHeight, juce::Font(fontHeight).getStringWidth(juce::String(getValue())), fontHeight, juce::Justification::centred);
            g.drawText(units, x + juce::Font(fontHeight).getStringWidth("0.000000"), y + fontHeight, juce::Font(fontHeight).getStringWidth(units), fontHeight, juce::Justification::centredRight); // temp value, figure out rest of layout first
            
        };
    private:
        CustomLookAndFeel lnf;
        juce::String units;
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