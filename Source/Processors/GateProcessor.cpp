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