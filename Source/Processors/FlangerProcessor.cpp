/*
  ==============================================================================

    FlangerProcessor.cpp
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#include "FlangerProcessor.h"

FlangerProcessor::FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts) : apvts(_apvts) {
    delayParam = apvts.getRawParameterValue(FLANGER_DELAY_STRING);
    onStateParam = apvts.getRawParameterValue(FLANGER_ON_STRING);
    mixParam = apvts.getRawParameterValue(FLANGER_MIX_STRING);
}
void FlangerProcessor::processBlock(juce::AudioBuffer<float>& buffer, int totalNumInputChannels, int sampleRate) {
    if (*onStateParam) {
        int numSamples = buffer.getNumSamples();
        float delayInSamples = *delayParam / float(sampleRate);
        for (int channel = 0; channel < totalNumInputChannels; channel++) {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sampleNum = 0; sampleNum < delayInSamples; sampleNum++) {

                float flangeSample = 1;

                float drySample = (1 - *mixParam / 100) * channelData[sampleNum];
                float wetSample = ((*mixParam) / 100) * flangeSample;
                channelData[sampleNum] = drySample + wetSample;
            }
        }
    }

}