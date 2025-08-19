/*
  ==============================================================================

    UtilityProcessor.cpp
    Created: 19 Aug 2025 5:27:13pm
    Author:  romal

  ==============================================================================
*/

#include "UtilityProcessor.h"
UtilityProcessor::UtilityProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex) : SwappableProcessor(bandIndex, processorIndex), apvts(_apvts) {
    assignParamPointers(processorIndex);
}

void UtilityProcessor::assignParamPointers(int index) {
    if (bandIndex >= 0) {
        onStateParam = apvts.getRawParameterValue(makeMultibandParamID(UTILITY_ON_STRING, bandIndex, index));
        volumeParam = apvts.getRawParameterValue(makeMultibandParamID(UTILITY_VOLUME_STRING, bandIndex, index));
        stereoParam = apvts.getRawParameterValue(makeMultibandParamID(UTILITY_STEREO_STRING, bandIndex, index));
    }
    else {

    }
}

void UtilityProcessor::moveParamValues(int index) {

    //cache current values
    const auto on = getOnState();
    const auto volume = getVolume();
    const auto stereo = getStereo();

    //reset currently pointed to values
    setOnState(false);
    setVolume(0);
    setStereo(0);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setVolume(volume);
    setStereo(stereo);

}

void UtilityProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* utilityProcessor = dynamic_cast<UtilityProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_volume = getVolume();
        const auto a_stereo = getStereo();

        const auto b_on = utilityProcessor->getOnState();
        const auto b_volume = utilityProcessor->getVolume();
        const auto b_stereo = utilityProcessor->getStereo();

        //swap values
        setOnState(b_on);
        setVolume(b_volume);
        setStereo(b_stereo);

        utilityProcessor->setOnState(a_on);
        utilityProcessor->setVolume(a_volume);
        utilityProcessor->setStereo(a_stereo);
    }
}

void UtilityProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (onStateParam != nullptr && volumeParam != nullptr && stereoParam != nullptr) {
        if (*onStateParam) {
            int numSamples = buffer.getNumSamples();
            float gain = juce::Decibels::decibelsToGain(volumeParam->load());
            for (int channel = 0; channel < 2; channel++) {
                auto* channelData = buffer.getWritePointer(channel);
                for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
                    channelData[sampleNum] *= gain;
                }
            }
            auto* leftData = buffer.getWritePointer(0);
            auto* rightData = buffer.getWritePointer(1);
            for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
                float leftSample = leftData[sampleNum];
                float rightSample = rightData[sampleNum];
                //TODO: check bug where audio doesnt fully pan
                if (*stereoParam > 0) {
                    //pan right
                    rightData[sampleNum] = leftSample * (1 - fabs(*stereoParam) / 50) + rightSample * (fabs(*stereoParam) / 50);
                    leftData[sampleNum] = leftSample * (1 - fabs(*stereoParam) / 50);
                }
                else if (*stereoParam < 0) {
                    //pan left
                    leftData[sampleNum] = rightSample * (1 - fabs(*stereoParam) / 50) + leftSample * (fabs(*stereoParam) / 50);
                    rightData[sampleNum] = rightSample * (1 - fabs(*stereoParam) / 50);
                }
            }
        }
    }

}

void UtilityProcessor::setOnState(bool value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(makeMultibandParamID(UTILITY_ON_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value ? 1.0f : 0.0f);
            param->endChangeGesture();
        }
    }
    else {
    }
}

void UtilityProcessor::setVolume(float value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
            apvts.getParameter(makeMultibandParamID(UTILITY_VOLUME_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(value));
            param->endChangeGesture();
        }
    }
    else {
    }
}

void UtilityProcessor::setStereo(float value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
            apvts.getParameter(makeMultibandParamID(UTILITY_STEREO_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
            param->endChangeGesture();
        }
    }
    else {
    }
}