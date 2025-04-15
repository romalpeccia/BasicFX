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
    auto labelBounds = bounds.withTrimmedRight(bounds.getWidth() * 0.333).withTrimmedLeft(bounds.getWidth() * 0.333);
    dbLabel.setBounds(labelBounds.withTrimmedTop(bounds.getHeight() * 0.8));
    meterBar.setBounds(bounds.withTrimmedBottom(bounds.getHeight() * 0.2));
}

void MeterBar::paint(juce::Graphics& g)  {
    auto bounds = getLocalBounds();
    float centreX = bounds.getCentreX();
    float height = bounds.getHeight();
    float startY = bounds.getBottom();;

    float valueRatio = (value + 100) / 100;
    float incrementalBarHeight = height / numIncrements;

    for (int i = 0; i < numIncrements; i++) {
        float barY = startY - incrementalBarHeight * i;
        float segmentRatio = i / numIncrements;
        if (segmentRatio < valueRatio) {
            g.setColour(juce::Colours::lawngreen.brighter(segmentRatio));
            g.fillRect(centreX - barWidth / 2, barY, barWidth, incrementalBarHeight);
        }
    }
}