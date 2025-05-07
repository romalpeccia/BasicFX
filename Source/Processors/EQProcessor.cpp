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
    prepareToPlay(sampleRate, totalNumInputChannels);
}

void EQProcessor::assignParamPointers(int index) {
    onStateParam = apvts.getRawParameterValue(makeID(EQ_ON_STRING, index));
    lowFrequencyParam = apvts.getRawParameterValue(makeID(EQ_LOW_FREQUENCY_STRING, index));
    highFrequencyParam = apvts.getRawParameterValue(makeID(EQ_HIGH_FREQUENCY_STRING, index));
    eqTypeParam = apvts.getRawParameterValue(makeID(EQ_TYPE_STRING, index));
}

void EQProcessor::moveParamValues(int index) {
    //cache current values
    const auto on = getOnState();
    const auto low_freq = getLowFrequency();
    const auto high_freq = getHighFrequency();
    const auto type = getEQType();

    //reset currently pointed to values
    setOnState(false);
    setLowFrequency(0);
    setHighFrequency(0);
    setEQType(0);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setLowFrequency(low_freq);
    setHighFrequency(high_freq);
    setEQType(type);

}

void EQProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* eqProcessor = dynamic_cast<EQProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_low_freq = getLowFrequency();
        const auto a_high_freq = getHighFrequency();
        const auto a_type = getEQType();

        const auto b_on = eqProcessor->getOnState();
        const auto b_low_freq = eqProcessor->getLowFrequency();
        const auto b_high_freq = eqProcessor->getHighFrequency();
        const auto b_type = eqProcessor->getEQType();

        //swap values
        setOnState(b_on);
        setLowFrequency(b_low_freq);
        setHighFrequency(b_high_freq);
        setEQType(b_type);

        eqProcessor->setOnState(a_on);
        eqProcessor->setLowFrequency(a_low_freq);
        eqProcessor->setHighFrequency(a_high_freq);
        eqProcessor->setEQType(a_type);
    }
}

void EQProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (onStateParam != nullptr && lowFrequencyParam != nullptr && highFrequencyParam != nullptr && eqTypeParam != nullptr && *onStateParam) {
        juce::dsp::AudioBlock<float> block(buffer);
         
        for (int ch = 0; ch < highPassFilters.size(); ch++)
        {
            auto channelBlock = block.getSingleChannelBlock(ch);
            juce::dsp::ProcessContextReplacing<float> context(channelBlock);
            DBG(*eqTypeParam);
            switch (int(*eqTypeParam)) {
                case 0:
                    lowPassFilters[ch].process(context);
                    break;
                case 1:
                    bandPassFilters[ch].process(context);
                    break;
                case 2:
                    highPassFilters[ch].process(context); 
                    break;
            }
        }   
    }
}

void EQProcessor::prepareToPlay(double _sampleRate, int _totalNumInputChannels) {
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = _sampleRate;
    spec.maximumBlockSize = _sampleRate;
    spec.numChannels = _totalNumInputChannels;
    
    lowPassFilters.resize(_totalNumInputChannels);
    highPassFilters.resize(_totalNumInputChannels);
    bandPassFilters.resize(_totalNumInputChannels);

    for (int ch = 0; ch < _totalNumInputChannels; ch++)
    {
        lowPassFilters[ch].prepare(spec);
        highPassFilters[ch].prepare(spec);
        bandPassFilters[ch].prepare(spec);
    }
    updateFilters();
}

void EQProcessor::updateFilters()
{
    auto hpCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighPass(sampleRate, *highFrequencyParam);
    auto lpCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(sampleRate, *lowFrequencyParam);

    for (auto& filter : highPassFilters)
        *filter.state = *hpCoeffs;

    for (auto& filter : lowPassFilters)
        *filter.state = *lpCoeffs;

    for (auto& chain : bandPassFilters)
    {
        *chain.get<0>().state = *lpCoeffs; 
        *chain.get<1>().state = *hpCoeffs; 
    }
}

void EQProcessor::actionListenerCallback(const juce::String& message) {
    updateFilters();

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

void EQProcessor::setHighFrequency(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(EQ_HIGH_FREQUENCY_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
    updateFilters();
}

void EQProcessor::setLowFrequency(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(EQ_LOW_FREQUENCY_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
    updateFilters();
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