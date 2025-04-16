/*
  ==============================================================================

    DBMeterComponent.h
    Created: 15 Apr 2025 12:14:43am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Processors/DBMeterProcessor.h"
#include "../Components/CustomSlider.h"

class MeterBar : public juce::Component {
public:
    MeterBar() {}
    ~MeterBar() {}

    void paint(juce::Graphics& g) override;
    void setValue(float _value) { value = _value; }

private:
    float numIncrements = 10; //how many increments the meter bar is divided into
    float value = -100.f;
    const juce::Colour BAR_COLOUR = juce::Colours::lawngreen;
    const juce::Colour INTERPOLATED_COLOUR = juce::Colours::lightgoldenrodyellow;
};

class DBMeterComponent : public juce::Component, public juce::Timer {

    public:
        DBMeterComponent(juce::AudioProcessorValueTreeState& _apvts, DBMeterProcessor& _processor);
        ~DBMeterComponent() {  }

        void timerCallback() override;
        void resized() override;
        void paint(juce::Graphics& g) override {}
        int getDBValue() { return dbValue; }
        
    private:
        juce::AudioProcessorValueTreeState& apvts;
        DBMeterProcessor& processor; 
        MeterBar meterBar;
        juce::Label dbLabel;
        const juce::Colour MAIN_COLOUR = juce::Colours::lawngreen;
        const juce::Colour SECONDARY_COLOUR = juce::Colours::white;
        CustomSlider rateSlider{ "Rate", "ms", MAIN_COLOUR,  SECONDARY_COLOUR };
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> rateSliderAttachment;

        float dbValue = -100.f;
        int minFontHeight = 5;

};

