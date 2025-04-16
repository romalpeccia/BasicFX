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

bool CustomSlider::hitTest(int x, int y) {
    //the slider is bound by the minimum of its width and height, overriding this function to make it so it only counts as a mouseclick if the actual slider drawing is clicked
    auto bounds = getLocalBounds().toFloat();
    float side = juce::jmin(bounds.getWidth(), bounds.getHeight());
    auto center = bounds.getCentre();
    float radius = side / 2.0f;

    auto dx = x - center.x;
    auto dy = y - center.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

void CustomSlider::paint(juce::Graphics& g) {
    auto startAngle = juce::degreesToRadians(180.f + 45.f);
    auto endAngle = juce::degreesToRadians(180.f - 45.f) + MathConstants<float>::twoPi;
    auto range = getRange();
    auto bounds = getLocalBounds();
    auto height = bounds.getHeight();
    auto width = bounds.getWidth();
    auto x = bounds.getX();
    auto y = bounds.getY();
    getLookAndFeel().drawRotarySlider(g, x, y, width, height, jmap(getValue(), range.getStart(), range.getEnd(), 0.0, 1.0), startAngle, endAngle, *this);

    //TODO: make this less hacky / nonpriority -  make text editable?
    int textAreaWidth = bounds.getWidth() / 2;
    int textAreaHeight = (bounds.getHeight() > textAreaWidth) ? textAreaWidth / 2 : bounds.getHeight() / 2;
    x = bounds.getCentre().getX() - textAreaWidth / 2 - textAreaWidth / 10;
    y = bounds.getCentre().getY() - textAreaHeight / 2;
    int fontHeight = textAreaHeight / 2;
    fontHeight = (fontHeight > minFontHeight) ? fontHeight : minFontHeight;
    g.setFont(fontHeight);
    g.drawText(getName(), x, y, juce::Font(fontHeight).getStringWidth(getName()), fontHeight, juce::Justification::centred);
    float value = getValue();
    if (units == "ms") {
        value = value * 1000;
    }
    else if (units == "db") {
        value = juce::Decibels::gainToDecibels(value);
    }
    g.drawText(juce::String(value), x, y + fontHeight, juce::Font(fontHeight).getStringWidth(juce::String(value)), fontHeight, juce::Justification::centred);
    g.drawText(units, x + juce::Font(fontHeight).getStringWidth("0.000000"), y + fontHeight, juce::Font(fontHeight).getStringWidth(units), fontHeight, juce::Justification::centredRight);
};