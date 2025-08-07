/*
  ==============================================================================

    FlangerProcessor.cpp
    Created: 13 Apr 2025 11:04:32pm
    Author:  romal

  ==============================================================================
*/

#include "FlangerProcessor.h"
/*
FlangerProcessor::FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts, int index) : SwappableProcessor(index), apvts(_apvts) {
    assignParamPointers(index);
    prepareToPlay(sampleRate, totalNumInputChannels);
}*/
FlangerProcessor::FlangerProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex) : SwappableProcessor(bandIndex, processorIndex), apvts(_apvts) {
    assignParamPointers(processorIndex);
    prepareToPlay(sampleRate, totalNumInputChannels);
}
void FlangerProcessor::assignParamPointers(int index) {
    if (bandIndex >= 0) {
        delayParam = apvts.getRawParameterValue(makeMultibandParamID(FLANGER_DELAY_STRING, bandIndex, index));
        onStateParam = apvts.getRawParameterValue(makeMultibandParamID(FLANGER_ON_STRING, bandIndex, index));
        mixParam = apvts.getRawParameterValue(makeMultibandParamID(FLANGER_MIX_STRING, bandIndex, index));
    }
    else {
        /*
        delayParam = apvts.getRawParameterValue(makeID(FLANGER_DELAY_STRING, index));
        onStateParam = apvts.getRawParameterValue(makeID(FLANGER_ON_STRING, index));
        mixParam = apvts.getRawParameterValue(makeID(FLANGER_MIX_STRING, index));
        */
    }
}

void FlangerProcessor::moveParamValues(int index) {
    //cache current values
    const auto on = getOnState();
    const auto delay = getDelay();
    const auto mix = getMix();

    //reset currently pointed to values
    setOnState(false);
    setDelay(0.0f);
    setMix(0.0f);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setDelay(delay);
    setMix(mix);
}

void FlangerProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* flangerProcessor = dynamic_cast<FlangerProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_delay = getDelay();
        const auto a_mix = getMix();

        const auto b_on = flangerProcessor->getOnState();
        const auto b_delay = flangerProcessor->getDelay();
        const auto b_mix = flangerProcessor->getMix();

        //swap values
        setOnState(b_on);
        setDelay(b_delay);
        setMix(b_mix);

        flangerProcessor->setOnState(a_on);
        flangerProcessor->setDelay(a_delay);
        flangerProcessor->setMix(a_mix);
    }
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
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterBool*>(
            apvts.getParameter(makeMultibandParamID(FLANGER_ON_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value ? 1.0f : 0.0f);
            param->endChangeGesture();
        }
    }
    else {
        /*
        if (auto* param = dynamic_cast<juce::AudioParameterBool*>(
            apvts.getParameter(makeID(FLANGER_ON_STRING, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value ? 1.0f : 0.0f);
            param->endChangeGesture();
        }*/
    }
}

void FlangerProcessor::setDelay(float value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
            apvts.getParameter(makeMultibandParamID(FLANGER_DELAY_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(value));
            param->endChangeGesture();
        }
    }
    else {/*
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(FLANGER_DELAY_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }*/
    }
}

void FlangerProcessor::setMix(float value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
            apvts.getParameter(makeMultibandParamID(FLANGER_MIX_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(value));
            param->endChangeGesture();
        }
    }
    else {/*
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(FLANGER_MIX_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }*/
    }
}