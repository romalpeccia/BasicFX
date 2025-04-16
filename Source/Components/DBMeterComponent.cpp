/*
  ==============================================================================

    DBMeterComponent.cpp
    Created: 15 Apr 2025 12:14:43am
    Author:  romal

  ==============================================================================
*/

#include "DBMeterComponent.h"

DBMeterComponent::DBMeterComponent(DBMeterProcessor& _processor) : processor(_processor) {
    startTimer(REFRESH_RATE_MS);
    addAndMakeVisible(dbLabel);
    addAndMakeVisible(meterBar);
    addAndMakeVisible(rateSlider);
}

void DBMeterComponent::timerCallback() {
    dbValue = processor.getDB();
    auto dbString = juce::String(dbValue);
    if (dbString == "-100") {
        dbString = "-inf";
    }
    dbLabel.setText(dbString + " db", juce::NotificationType::sendNotification);
    meterBar.setValue(dbValue);
    repaint();
}

void DBMeterComponent::resized() {
    auto bounds = getLocalBounds();
    auto height = bounds.getHeight();
    auto width = bounds.getWidth();
    auto labelBounds = bounds.withTrimmedTop(height * 0.8);
    dbLabel.setBounds(labelBounds);
    meterBar.setBounds(bounds.withTrimmedBottom(height * 0.2));
    rateSlider.setBounds(bounds.withTrimmedBottom(height * 0.6).withTrimmedRight(width * 0.6));
}

void DBMeterComponent::MeterBar::paint(juce::Graphics& g)  {
    auto bounds = getLocalBounds();
    float centreX = bounds.getCentreX();
    float height = bounds.getHeight();
    float startY = bounds.getBottom();;
    float barWidth = bounds.getWidth();
    float valueRatio = (value + 100) / 100;
    float incrementalBarHeight = height / numIncrements;

    for (int i = 0; i < numIncrements; i++) {
        float barY = startY - incrementalBarHeight * i;
        float segmentRatio = float(i) / numIncrements;
        if (segmentRatio < valueRatio) {
            g.setColour(BAR_COLOUR.brighter(segmentRatio).interpolatedWith(INTERPOLATED_COLOUR, segmentRatio));
            g.fillRect(centreX - barWidth / 2, barY, barWidth, incrementalBarHeight);
        }
    }
}