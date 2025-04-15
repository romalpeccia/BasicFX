/*
  ==============================================================================

    DBMeterProcessor.h
    Created: 15 Apr 2025 12:23:35am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utilities.h"

class DBMeterProcessor {
    public:
        DBMeterProcessor() {}
        void processBlock(juce::AudioBuffer<float>& buffer) {
            float peak = juce::jmax(buffer.getMagnitude(0, 0, buffer.getNumSamples()), buffer.getMagnitude(1, 0, buffer.getNumSamples()));
            db = juce::Decibels::gainToDecibels(peak);
        }
        float getDB() { return db; }
    private:
        float db = -100.f;
};