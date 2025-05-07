/*
  ==============================================================================

    GateProcessor.cpp
    Created: 13 Apr 2025 11:04:40pm
    Author:  romal

  ==============================================================================
*/

#include "GateProcessor.h"


GateProcessor::GateProcessor(juce::AudioProcessorValueTreeState& _apvts, int index) : SwappableProcessor(index), apvts(_apvts) {
    assignParamPointers(index);
}

void GateProcessor::assignParamPointers(int index){

    onStateParam = apvts.getRawParameterValue(makeID(GATE_ON_STRING, index));
    thresholdParam = apvts.getRawParameterValue(makeID(THRESHOLD_STRING, index));
    gateTypeParam = apvts.getRawParameterValue(makeID(GATE_STATE_STRING, index));
    attackParam = apvts.getRawParameterValue(makeID(ATTACK_STRING, index));
    releaseParam = apvts.getRawParameterValue(makeID(RELEASE_STRING, index));
    holdParam = apvts.getRawParameterValue(makeID(HOLD_STRING, index));

}

void GateProcessor::moveParamValues(int index){
    //cache current values
    const auto on = getOnState();
    const auto thresh = getThreshold();
    const auto type = getGateType();
    const auto attack = getAttack();
    const auto release = getRelease();
    const auto hold = getHold();

    //reset currently pointed to values
    setOnState(false);
    setThreshold(0.0f);
    setGateType(0);
    setAttack(0.0f);
    setRelease(0.0f);
    setHold(0.0f);

    //change index and move pointers to new index
    setProcessorIndex(index);
    assignParamPointers(index);

    //overwrite newly pointed to values
    setOnState(on);
    setThreshold(thresh);
    setGateType(type);
    setAttack(attack);
    setRelease(release);
    setHold(hold);
}

void GateProcessor::swapParamValues(SwappableProcessor* otherProcessor)
{
    if (auto* gateProcessor = dynamic_cast<GateProcessor*>(otherProcessor)) {
        //cache current values
        const auto a_on = getOnState();
        const auto a_thresh = getThreshold();
        const auto a_type = getGateType();
        const auto a_attack = getAttack();
        const auto a_release = getRelease();
        const auto a_hold = getHold();

        const auto b_on = gateProcessor->getOnState();
        const auto b_thresh = gateProcessor->getThreshold();
        const auto b_type = gateProcessor->getGateType();
        const auto b_attack = gateProcessor->getAttack();
        const auto b_release = gateProcessor->getRelease();
        const auto b_hold = gateProcessor->getHold();

        //swap values
        setOnState(b_on);
        setThreshold(b_thresh);
        setGateType(b_type);
        setAttack(b_attack);
        setRelease(b_release);
        setHold(b_hold);

        gateProcessor->setOnState(a_on);
        gateProcessor->setThreshold(a_thresh);
        gateProcessor->setGateType(a_type);
        gateProcessor->setAttack(a_attack);
        gateProcessor->setRelease(a_release);
        gateProcessor->setHold(a_hold);
    }
}

void GateProcessor::processBlock(juce::AudioBuffer <float>& buffer) {
    if (onStateParam != nullptr && thresholdParam != nullptr && gateTypeParam != nullptr && attackParam != nullptr && releaseParam != nullptr && holdParam != nullptr) {
        if (*onStateParam) {
            switch (int(*gateTypeParam)) {
            case 0:
                processGateSimple(buffer);
                break;
            case 1:
                processGateMedium(buffer);
                break;
            case 2:
                processGateAdvanced(buffer);
                break;
            }
        }
    }
}

void GateProcessor::processGateSimple(juce::AudioBuffer<float>& buffer) {
    int numSamples = buffer.getNumSamples();

    float* channelData[MAX_INPUT_CHANNELS];
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        channelData[channel] = buffer.getWritePointer(channel);
    }

    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        bool triggerGate = false;
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            if (abs(channelData[channel][sampleNum]) < *thresholdParam) {
                triggerGate = true;
                break;
            }
        }
        if (triggerGate == true) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] = 0;
            }
        }
    }
}

void GateProcessor::processGateMedium(juce::AudioBuffer<float>& buffer) {
    int numSamples = buffer.getNumSamples();

    if (calculateRMSAcrossChannels(buffer) < *thresholdParam) {
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
                channelData[sampleNum] = 0;
            }
        }
    }
}

void GateProcessor::processGateAdvanced(juce::AudioBuffer<float>& buffer) {

    int numSamples = buffer.getNumSamples();
    float attack = *attackParam;
    float release = *releaseParam;
    float hold = *holdParam;

    //TODO: handle changes in params from UI where sampleCounters become greater than the samplesPerWindow, causing extreme loudness

    int samplesPerAttackWindow = attack * sampleRate;
    int samplesPerReleaseWindow = release * sampleRate;
    int samplesPerHoldWindow = hold * sampleRate;


    float rms = calculateRMSAcrossChannels(buffer);
    bool gateCondition = (rms < *thresholdParam && rms != 0); //TODO: choose a better condition
    if (gateState == GateState::Open && gateCondition) {
        //gate is closed, check incoming values and set attack based on some trigger condition
        gateState = GateState::Attack;
        attackStartRatio = 1.0f;
        attackSampleCounter = samplesPerAttackWindow;

    }
    if (gateCondition) {
        // handle if the is retriggered? do I need this?
        /*
        if (gateState == GateState::Attack) {
            //extend the next hold phase
            if (retriggerHoldSampleCounter == 0) {
                retriggerHoldSampleCounter = std::max(0, samplesPerAttackWindow - attackSampleCounter);
            }
        }
        else if (gateState == GateState::Hold) {
            //extend the current hold phase
            
                if (holdSampleCounter < samplesPerHoldWindow) {
                    holdSampleCounter = samplesPerHoldWindow;
                }
        }
        if (gateState == GateState::Release) {
            // start a new attack phase based on where the release window is

            gateState = GateState::Attack;
            float ratio = 1.0f - float(releaseSampleCounter) / float(samplesPerReleaseWindow);
            attackStartRatio = 1- std::pow(ratio, 2.0f);
            attackSampleCounter = int(ratio *samplesPerAttackWindow);

        }
        */
    }

    //make pointers to the buffer channels for easy access
    float* channelData[MAX_INPUT_CHANNELS];
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        channelData[channel] = buffer.getWritePointer(channel);
    }

    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        if (gateState == GateState::Attack) {
            float ratio = 1.0f - float(attackSampleCounter) / float(samplesPerAttackWindow);
            float smoothedRatio = (1.0f - std::pow(ratio, 2.0f));
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] *= attackStartRatio * smoothedRatio;
            }
            attackSampleCounter--;

            if (attackSampleCounter <= 0) {
                gateState = GateState::Hold;
                holdSampleCounter = samplesPerHoldWindow + retriggerHoldSampleCounter;
                retriggerHoldSampleCounter = 0;
            }
        }

        if (gateState == GateState::Hold) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] = 0;
            }
            holdSampleCounter--;
            if (holdSampleCounter <= 0) {
                gateState = GateState::Release;
                releaseSampleCounter = samplesPerReleaseWindow;
            }
        }

        if (gateState == GateState::Release) {
            float ratio = 1.0f - float(releaseSampleCounter) / float(samplesPerReleaseWindow);
            float smoothedRatio = std::pow(ratio, 2.0f);
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] *= smoothedRatio;
            }
            releaseSampleCounter--;
            if (releaseSampleCounter <= 0) {
                gateState = GateState::Open;
            }
        }
        else {
            //sample unchanged
        }
    }


}






void GateProcessor::setOnState(bool value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterBool*>(
        apvts.getParameter(makeID(GATE_ON_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(value ? 1.0f : 0.0f);
        param->endChangeGesture();
    }
}

void GateProcessor::setThreshold(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(THRESHOLD_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}

void GateProcessor::setGateType(int value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterChoice*>(
        apvts.getParameter(makeID(GATE_STATE_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(value)));
        param->endChangeGesture();
    }
}

void GateProcessor::setAttack(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(ATTACK_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}

void GateProcessor::setRelease(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(RELEASE_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}

void GateProcessor::setHold(float value)
{
    if (auto* param = dynamic_cast<juce::AudioParameterFloat*>(
        apvts.getParameter(makeID(HOLD_STRING, getProcessorIndex()))))
    {
        param->beginChangeGesture();
        param->setValueNotifyingHost(param->convertTo0to1(value));
        param->endChangeGesture();
    }
}