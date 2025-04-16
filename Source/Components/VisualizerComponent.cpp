/*
  ==============================================================================

    VisualizerComponent.cpp
    Created: 15 Apr 2025 1:52:06pm
    Author:  romal

  ==============================================================================
*/

#include "VisualizerComponent.h"

VisualizerComponent::VisualizerComponent(VisualizerProcessor& _processor1, VisualizerProcessor& _processor2)
    : processor1(_processor1), processor2(_processor2) {
    startTimer(REFRESH_RATE_MS);

}

void VisualizerComponent::timerCallback() {
    repaint();
}

void VisualizerComponent::paint(juce::Graphics& g) {
    auto bounds = getLocalBounds();
    g.fillAll(juce::Colours::white);
    int centreY = bounds.getCentreY();

    //draw the axes
    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    g.drawLine(0, bounds.getY(), 0, bounds.getBottom());
    g.drawLine(0, centreY, bounds.getRight(), centreY);

    paintSignal(g, processor1, juce::Colours::black, juce::Colours::grey );
    paintSignal(g, processor2, juce::Colours::red, juce::Colours::blue );
};

void VisualizerComponent::paintSignal(juce::Graphics& g, VisualizerProcessor& processor, juce::Colour peakColour, juce::Colour bodyColour) {
    //finds points based on values from processor and adds them to a path, then draws the path
    //TODO: bodyColour logic  

    auto bounds = getLocalBounds();
    int centreY = bounds.getCentreY();
    juce::Path waveformPath;
    bool firstPoint = true;

    for (int i = 0; i < bounds.getWidth(); i++) {
        float value = processor.readSample(0, i);
        int valueLength = int(value * bounds.getHeight() / 2);
        int lineY = centreY - valueLength;

        //add the point to the path;
        if (firstPoint) {
            waveformPath.startNewSubPath((float)i, (float)lineY);
            firstPoint = false;
        }
        else {
            waveformPath.lineTo((float)i, (float)lineY);
        } 
        //draw the sample body
        /*
        g.setColour(bodyColour.withAlpha(0.1f));
        if (lineY < centreY && valueLength != 0) {
            g.fillRect(i, lineY, 1, centreY - lineY);
        }
        else if (lineY > centreY && valueLength != 0) {
            g.fillRect(i, bounds.getCentreY(), 1, lineY - centreY);
        }
        */
    }
    //draw the waveform
    g.setColour(peakColour.withAlpha(0.5f));
    g.strokePath(waveformPath, juce::PathStrokeType(1.f));
}