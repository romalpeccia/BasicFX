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
        VisualizerComponent(VisualizerProcessor& _processor1, VisualizerProcessor& _processor2)
            : processor1(_processor1), processor2(_processor2) {
            startTimer(REFRESH_RATE_MS);
            processor1.setMaxSamples(getLocalBounds().getWidth());
            processor2.setMaxSamples(getLocalBounds().getWidth());
        }
        ~VisualizerComponent() {  }
        void timerCallback() override {
            processor1.sampleBuffer.write(0, processor1.getCurrentValue());
            processor2.sampleBuffer.write(0, processor2.getCurrentValue());
            repaint();
        }
        void resized() override {
            processor1.setMaxSamples(getLocalBounds().getWidth());
            processor2.setMaxSamples(getLocalBounds().getWidth());
        };
        void paint(juce::Graphics& g) override {
            auto bounds = getLocalBounds();
            int centreY = bounds.getCentreY();
            //draw the axes
            g.setColour(juce::Colours::white);
            g.drawLine(0, bounds.getY(), 0, bounds.getBottom());
            g.drawLine(0, centreY, bounds.getRight(), bounds.getCentreY());

            paintSignal(g, processor1, juce::Colours::grey, juce::Colours::black);
            paintSignal(g, processor2, juce::Colours::lightgreen, juce::Colours::green);
        };

        void paintSignal(juce::Graphics& g, VisualizerProcessor& processor, juce::Colour bodyColour, juce::Colour peakColour) {
            auto bounds = getLocalBounds();
            int centreY = bounds.getCentreY();
            for (int i = 0; i < bounds.getWidth(); i++) {
                float value = processor.sampleBuffer.read(0, i);
                int valueLength = int(value * bounds.getHeight());
                int lineY = valueLength + bounds.getCentreY();

                //draw the sample body
                g.setColour(bodyColour.withAlpha(0.5f));
                if (lineY < centreY && valueLength != 0) {
                    g.fillRect(i, lineY, 1, centreY - lineY);
                }
                else if (lineY > centreY && valueLength != 0) {
                    g.fillRect(i, bounds.getCentreY(), 1, lineY - centreY);
                }
                //draw the sample peak
                g.setColour(peakColour);
                g.fillRect(i, lineY, 1, 1);

            }
        }

        VisualizerProcessor& processor1;
        VisualizerProcessor& processor2;
    private:
};