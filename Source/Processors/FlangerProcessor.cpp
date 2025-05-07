/*
  ==============================================================================

    FlangerProcessor.cpp
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#include "FlangerProcessor.h"

FlangerProcessor::FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts, int index) : SwappableProcessor(index), apvts(_apvts) {
    assignParamPointers(index);
    prepareToPlay(sampleRate, totalNumInputChannels);
}
void FlangerProcessor::assignParamPointers(int index) {
    delayParam = apvts.getRawParameterValue(makeID(FLANGER_DELAY_STRING, index));
    onStateParam = apvts.getRawParameterValue(makeID(FLANGER_ON_STRING, index));
    mixParam = apvts.getRawParameterValue(makeID(FLANGER_MIX_STRING, index));
}


void FlangerProcessor::processBlock(juce::AudioBuffer<float>& buffer) {
    if (delayParam != nullptr && onStateParam != nullptr && mixParam != nullptr) {
        if (*onStateParam) {
            int numSamples = buffer.getNumSamples();
            float delayInSamples = *delayParam * float(sampleRate);
            for (int channel = 0; channel < totalNumInputChannels; channel++) {
                auto* channelData = buffer.getWritePointer(channel);
                for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {

                    sampleBuffer.write(channel, channelData[sampleNum]);
                    float flangeSample = sampleBuffer.readInterpolated(channel, delayInSamples);
                    float wetSample = ((*mixParam) / 100) * flangeSample;
                    float drySample = (1 - *mixParam / 100) * channelData[sampleNum];
                    channelData[sampleNum] = drySample + wetSample;
                }
            }
        }
    }
}

void FlangerProcessor::setOnState(bool value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterBool*>(
        apvts.getParameter(makeID(FLANGER_ON_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(value ? 1.0f : 0.0f);
        param->endChangeGesture();
    }
}

void FlangerProcessor::setDelay(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(FLANGER_DELAY_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}

void FlangerProcessor::setMix(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(FLANGER_MIX_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}