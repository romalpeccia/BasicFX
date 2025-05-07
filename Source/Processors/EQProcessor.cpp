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

void EQProcessor::moveParamValues(int index) {
    //cache current values
    const auto on = getOnState();
    const auto amt = getAmount();
    const auto type = getEQType();

    //reset currently pointed to values
    setOnState(false);
    setAmount(0);
    setEQType(0);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setAmount(amt);
    setEQType(type);

}

void EQProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* eqProcessor = dynamic_cast<EQProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_amt = getAmount();
        const auto a_type = getEQType();

        const auto b_on = eqProcessor->getOnState();
        const auto b_amt = eqProcessor->getAmount();
        const auto b_type = eqProcessor->getEQType();

        //swap values
        setOnState(b_on);
        setAmount(b_amt);
        setEQType(b_type);

        eqProcessor->setOnState(a_on);
        eqProcessor->setAmount(a_amt);
        eqProcessor->setEQType(a_type);
    }
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