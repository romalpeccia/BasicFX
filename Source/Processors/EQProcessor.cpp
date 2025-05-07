/*
  ==============================================================================

    EQProcessor.cpp
    Created: 6 May 2025 9:43:32pm
    Author:  romal

  ==============================================================================
*/

#include "EQProcessor.h"


EQProcessor::EQProcessor(juce::AudioProcessorValueTreeState& _apvts, int index) : SwappableProcessor(index), apvts(_apvts) {
    assignParamPointers(index);
}

void EQProcessor::assignParamPointers(int index) {
    onStateParam = apvts.getRawParameterValue(makeID(EQ_ON_STRING, index));
    amountParam = apvts.getRawParameterValue(makeID(EQ_AMOUNT_STRING, index));
    eqTypeParam = apvts.getRawParameterValue(makeID(EQ_TYPE_STRING, index));
}

void EQProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (onStateParam != nullptr && amountParam != nullptr && eqTypeParam != nullptr) {
        if (*onStateParam) {
            switch (int(*eqTypeParam)) {
                case 0:
                    break;
                case 1:
                    break;
                case 2:
                    break;
            }
        }
    }

}

void EQProcessor::setOnState(bool value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterBool*>(apvts.getParameter(makeID(EQ_ON_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(value ? 1.0f : 0.0f);
        param->endChangeGesture();
    }
}

void EQProcessor::setAmount(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(EQ_AMOUNT_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}

void EQProcessor::setEQType(int value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
        apvts.getParameter(makeID(EQ_TYPE_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
        param->endChangeGesture();
    }
}