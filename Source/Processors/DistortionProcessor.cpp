/*
  ==============================================================================

    DistortionProcessor.cpp
    Created: 13 Apr 2025 11:05:18pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionProcessor.h"


DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    onStateParam = apvts.getRawParameterValue(DISTORTION_ON_STRING);
    amountParam = apvts.getRawParameterValue(DISTORTION_AMOUNT_STRING);
    distortionTypeParam = apvts.getRawParameterValue(DISTORTION_TYPE_STRING);
}

void DistortionProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (*onStateParam) {
        switch (int(*distortionTypeParam)) {
        case 0:
            processDistortionWaveRectifier(buffer);
            break;
        case 1:
            processDistortionBitCrusher(buffer);
            break;
        case 2:
            processDistortionSoftClipperCubic(buffer);
            break;
        case 3:
            processDistortionSoftClipperArcTan(buffer);
            break;
        case 4:
            processDistortionSlewLimiter(buffer);
            break;
        }
    }
}

void DistortionProcessor::processDistortionWaveRectifier(juce::AudioBuffer<float>& buffer) {
    //at 0 behaves as a half-wave rectifier, at 1 behaves as a full-wave rectifier,
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            if (channelData[sampleNum] < 0) {
                channelData[sampleNum] *= *amountParam * -1;
            }
        }
    }
}
float bitCrush(float sample, int bits) {

    float stepSize = 2.0f / (bits - 1);
    float crushedSample = round(sample / stepSize) * stepSize;
    return juce::jlimit(-1.0f, 1.0f, crushedSample);
}

void DistortionProcessor::processDistortionBitCrusher(juce::AudioBuffer<float>& buffer) {
    //starts at 100 bits and goes down to 1 bit 
    int numBits = int((1 - *amountParam) * 100);
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            if (*amountParam != 0) {
                channelData[sampleNum] = bitCrush(channelData[sampleNum], numBits);
            }
        }
    }
}
void DistortionProcessor::processDistortionSoftClipperCubic(juce::AudioBuffer<float>& buffer) {
    //at distortion = 0 acts as a linear function (signal passes through normally), at distortion = 1 acts as a cubic function

    float constant = (1.f / 3.f); //TODO make this a knob?
    //constant = 1; //for testing
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            channelData[sampleNum] = channelData[sampleNum] + (*amountParam) * constant * pow(channelData[sampleNum], 3.f);
        }
    }
}
void DistortionProcessor::processDistortionSoftClipperArcTan(juce::AudioBuffer<float>& buffer) {
    // applies (2/pi) * arctan(alpha * x) where alpha is controlled by the distortion knob

    float alphaMultiplier = 100; //TODO make this a knob?
    float alpha = *amountParam * alphaMultiplier;
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            channelData[sampleNum] = (2 / juce::MathConstants<float>::pi) * std::atan(alpha * channelData[sampleNum]);

        }
    }
}
void DistortionProcessor::processDistortionSlewLimiter(juce::AudioBuffer<float>& buffer) {
    // limits rate of change between samples 
    int numSamples = buffer.getNumSamples();
    float maxDelta = 1 - *amountParam;
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        float prevSample;
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            float currSample = channelData[sampleNum];
            if (sampleNum == 0) {
                prevSample = lastSampleFromPrevBuffer[channel];
            }
            else {
                prevSample = channelData[sampleNum - 1];
            }

            float delta = currSample - prevSample;
            if (fabs(delta) > maxDelta) {
                if (delta > 0) {
                    //rising signal, limit rise
                    currSample = prevSample + juce::jlimit(-maxDelta, maxDelta, maxDelta);
                }
                else {
                    //falling signal, limit fall
                    currSample = prevSample - juce::jlimit(-maxDelta, maxDelta, maxDelta);
                }
                
            }
            channelData[sampleNum] = currSample;
        }
        lastSampleFromPrevBuffer[channel] = channelData[numSamples - 1];
    }
}