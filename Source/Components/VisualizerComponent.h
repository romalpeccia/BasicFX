/*
  ==============================================================================

    VisualizerComponent.h
    Created: 15 Apr 2025 1:52:06pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../Utilities.h"
#include "../Processors/VisualizerProcessor.h"

class VisualizerComponent : public juce::Component, public juce::Timer {
    public:
        VisualizerComponent(VisualizerProcessor& _processor1, VisualizerProcessor& _processor2);
        ~VisualizerComponent() {  }
        void timerCallback();
        void resized() override {};;
        void paint(juce::Graphics& g) override;
        void paintSignal(juce::Graphics& g, VisualizerProcessor& processor, juce::Colour bodyColour, juce::Colour peakColour);


    private:
        VisualizerProcessor& processor1;
        VisualizerProcessor& processor2;
};