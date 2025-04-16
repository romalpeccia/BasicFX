/*
  ==============================================================================

    CustomSlider.h
    Created: 14 Apr 2025 10:12:20pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utilities.h"

class CustomSlider : public juce::Slider {

public:
    CustomSlider(juce::String name, juce::String units, const juce::Colour mainColour, const juce::Colour secondaryColour);
    ~CustomSlider();

    juce::String getUnits() { return units; }
    void setUnits(juce::String _units) { units = _units; updateText(); }

    bool hitTest(int x, int y);

    void paint(juce::Graphics& g);

private:
    CustomLookAndFeel lnf;
    juce::String units;
    int minFontHeight = 5;
};
