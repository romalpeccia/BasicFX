/*
  ==============================================================================

    DistortionProcessor.cpp
    Created: 13 Apr 2025 11:05:18pm
    Author:  romal

  ==============================================================================
*/

#include "DistortionProcessor.h"

/*
DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts, int index) : SwappableProcessor(index), apvts(_apvts) {
    assignParamPointers(index);
}*/
DistortionProcessor::DistortionProcessor(juce::AudioProcessorValueTreeState& _apvts, int bandIndex, int processorIndex) : SwappableProcessor(bandIndex, processorIndex), apvts(_apvts) {
    assignParamPointers(processorIndex);
}

void DistortionProcessor::assignParamPointers(int index) {
    if (bandIndex >= 0) {
        onStateParam = apvts.getRawParameterValue(makeMultibandParamID(DISTORTION_ON_STRING, bandIndex, index));
        amountParam = apvts.getRawParameterValue(makeMultibandParamID(DISTORTION_AMOUNT_STRING, bandIndex, index));
        distortionTypeParam = apvts.getRawParameterValue(makeMultibandParamID(DISTORTION_TYPE_STRING, bandIndex, index));
    }
    else {
        /*
        onStateParam = apvts.getRawParameterValue(makeID(DISTORTION_ON_STRING, index));
        amountParam = apvts.getRawParameterValue(makeID(DISTORTION_AMOUNT_STRING, index));
        distortionTypeParam = apvts.getRawParameterValue(makeID(DISTORTION_TYPE_STRING, index));
        */
    }
}

void DistortionProcessor::moveParamValues(int index) {

    //cache current values
    const auto on = getOnState();
    const auto amt = getAmount();
    const auto type = getDistortionType();

    //reset currently pointed to values
    setOnState(false);
    setAmount(0);
    setDistortionType(0);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setAmount(amt);
    setDistortionType(type);

}

void DistortionProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* distortionProcessor = dynamic_cast<DistortionProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_amt = getAmount();
        const auto a_type = getDistortionType();

        const auto b_on = distortionProcessor->getOnState();
        const auto b_amt = distortionProcessor->getAmount();
        const auto b_type = distortionProcessor->getDistortionType();

        //swap values
        setOnState(b_on);
        setAmount(b_amt);
        setDistortionType(b_type);

        distortionProcessor->setOnState(a_on);
        distortionProcessor->setAmount(a_amt);
        distortionProcessor->setDistortionType(a_type);
    }
}

void DistortionProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (onStateParam != nullptr && amountParam != nullptr && distortionTypeParam != nullptr){
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

float DistortionProcessor::bitCrush(float sample, int bits) {

    float stepSize = 2.0f / (bits - 1);
    float crushedSample = round(sample / stepSize) * stepSize;
    return juce::jlimit(-1.0f, 1.0f, crushedSample);
}
void DistortionProcessor::setOnState(bool value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(makeMultibandParamID(DISTORTION_ON_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(value ? 1.0f : 0.0f);
            param->endChangeGesture();
        }
    }
    else {/*
    if (auto* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(makeID(DISTORTION_ON_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(value ? 1.0f : 0.0f);
        param->endChangeGesture();
    }*/
    }
}

void DistortionProcessor::setAmount(float value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
            apvts.getParameter(makeMultibandParamID(DISTORTION_AMOUNT_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(value));
            param->endChangeGesture();
        }
    }
    else {/*
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(DISTORTION_AMOUNT_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }*/
    }
}

void DistortionProcessor::setDistortionType(int value)
{
    if (bandIndex >= 0) {
        if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
            apvts.getParameter(makeMultibandParamID(DISTORTION_TYPE_STRING, bandIndex, getProcessorIndex()))))
        {
            param->beginChangeGesture();
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
            param->endChangeGesture();
        }
    }
    else {/*
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
        apvts.getParameter(makeID(DISTORTION_TYPE_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
        param->endChangeGesture();
    }*/
    }
}