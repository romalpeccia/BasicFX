/*
  ==============================================================================

    CustomSlider.cpp
    Created: 14 Apr 2025 10:12:20pm
    Author:  romal

  ==============================================================================
*/

#include "CustomSlider.h"

CustomSlider::CustomSlider(juce::String name, juce::String units, const juce::Colour mainColour, const juce::Colour secondaryColour) {
    setName(name);
    setUnits(units);
    setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
    //setColour(juce::Slider::ColourIds::textBoxOutlineColourId, mainColour);
    //setColour(juce::Slider::ColourIds::textBoxTextColourId, secondaryColour.brighter(0.5f));
    //setColour(juce::Slider::ColourIds::textBoxBackgroundColourId, mainColour.darker(0.5f));
    setColour(juce::Slider::ColourIds::trackColourId, mainColour);
    setColour(juce::Slider::ColourIds::thumbColourId, secondaryColour);
    setColour(juce::Slider::ColourIds::backgroundColourId, COMPONENT_COLOUR_OFF);
    setSliderStyle(juce::Slider::SliderStyle::Rotary);
    setLookAndFeel(&lnf);

};

CustomSlider::~CustomSlider() { setLookAndFeel(nullptr); };

bool CustomSlider::hitTest(int x, int y)
{
    //x, y are the coordinates of the click that triggered this function
    //the slider is bound by the minimum of its width and height, overriding this function to make it so the UI only counts as a mouseclick if the actual slider drawing is clicked instead of anywhere on the component bounds
    auto bounds = getLocalBounds().toFloat();
    float side = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto center = bounds.getCentre();
    float radius = side / 2.0f;

    float lineW = juce::jmin(8.0f, radius * 0.5f); // match drawRotarySlider //TODO make this a class member in LNF so that we can reference it throughout the slider implementation

    auto dx = x - center.x;
    auto dy = y - center.y;
    float distanceFromCentre = std::sqrt(dx * dx + dy * dy);

    int margin = bounds.getWidth() / 10;
    float innerRadius = radius - lineW - margin;
    float outerRadius = radius + margin;

    return distanceFromCentre >= innerRadius && distanceFromCentre <= outerRadius;
}

void CustomSlider::paint(juce::Graphics& g) {

    auto bounds = getLocalBounds();
    //Draw the slider
    auto range = getRange();

    auto startAngle = juce::degreesToRadians(180.f + 45.f);
    auto endAngle = juce::degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    auto rotaryPos = juce::jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0);
    
    getLookAndFeel().drawRotarySlider(g, bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(), rotaryPos, startAngle, endAngle, *this);

    //Set up the value, label, and units
    float value = getValue();

    //scale the text bounds to be within the slider
    int size = juce::jmin(bounds.getHeight(), bounds.getWidth());
    auto sliderBounds = juce::Rectangle<float>(bounds.getCentreX() - size / 2, bounds.getCentreY() - size / 2, size, size);
    auto textBounds = sliderBounds.toFloat().getProportion(juce::Rectangle<float>(0.3, 0.3, 0.5, 0.5));
    auto x = textBounds.getX();
    auto y = textBounds.getY();
    
    int fontHeight = size / 8;
    fontHeight = juce::jmax(fontHeight, minFontHeight);
    auto font = juce::Font(fontHeight);
    g.setFont(font);

    juce::String valueString = juce::String(value);
    juce::String unitString = units;
    if (units == "s") {
        valueString = juce::String(value * 1000);
        unitString = "ms";
    }
    else if (units == "amplitude") {
        valueString = juce::String(juce::Decibels::gainToDecibels(value));
        unitString = "db";
    }

    //Draw the label, value, and units      TODO: non-priority make text editable?
    g.drawText(getName(), x, y, font.getStringWidth(getName()), fontHeight, juce::Justification::centred);
    g.drawText(juce::String(valueString), x, y + fontHeight, font.getStringWidth(juce::String(valueString)), fontHeight, juce::Justification::centred);
    g.drawText(unitString, x + font.getStringWidth("0.000000"), y + fontHeight, font.getStringWidth(unitString), fontHeight, juce::Justification::centredRight);
};

