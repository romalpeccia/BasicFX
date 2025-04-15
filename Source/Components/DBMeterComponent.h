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

class MeterBar : public juce::Component {
public:
    MeterBar() {}
    ~MeterBar() {}


    void paint(juce::Graphics& g) override;

    void setValue(float _value) { value = _value; }

private:
    float numIncrements = 100; //how many increments the meter bar is divided into
    float value = -100.f;
    float barWidth = 20;
};

class DBMeterComponent : public juce::Component, public juce::Timer {
    public:
        DBMeterComponent(DBMeterProcessor& _processor);
        ~DBMeterComponent() {  }

        void timerCallback() override;
        void resized() override;
        //void paint(juce::Graphics& g) override {}
        int getDBValue() { return dbValue; }


        DBMeterProcessor& processor; //TODO: make this private and return a pointer
        MeterBar meterBar;
    private:

        float dbValue = -100.f;
        int minFontHeight = 5;
        juce::Label dbLabel;
};

