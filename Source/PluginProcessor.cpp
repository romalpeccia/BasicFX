/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
using namespace std;
//==============================================================================
BasicFXAudioProcessor::BasicFXAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    gateOnParam = apvts.getRawParameterValue(GATE_ON_STRING);
    thresholdParam = apvts.getRawParameterValue(THRESHOLD_STRING);
    gateStateParam = apvts.getRawParameterValue(GATE_STATE_STRING);
    attackParam = apvts.getRawParameterValue(ATTACK_STRING);
    releaseParam = apvts.getRawParameterValue(RELEASE_STRING);
    holdParam = apvts.getRawParameterValue(HOLD_STRING);

    distortionOnParam = apvts.getRawParameterValue(DISTORTION_ON_STRING);
    distortionParam = apvts.getRawParameterValue(DISTORTION_STRING);
    distortionStateParam = apvts.getRawParameterValue(DISTORTION_STATE_STRING);

    flangerDelayParam = apvts.getRawParameterValue(FLANGER_DELAY_STRING);
    flangerOnParam = apvts.getRawParameterValue(FLANGER_ON_STRING);
    flangerMixParam = apvts.getRawParameterValue(FLANGER_MIX_STRING);

    /*
    cachedAttack = ATTACK_MIN;
    cachedHold = RELEASE_MIN;
    cachedRelease = HOLD_MIN;
    */
}

BasicFXAudioProcessor::~BasicFXAudioProcessor()
{
}


juce::AudioProcessorValueTreeState::ParameterLayout BasicFXAudioProcessor::createParameterLayout() {
    //Creates all the parameters that change based on the user input and returns them in a AudioProcessorValueTreeState::ParameterLayout object



    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(THRESHOLD_STRING, THRESHOLD_STRING.toLowerCase(), juce::NormalisableRange<float>(0, 1.f, 0.001, 1.f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(GATE_ON_STRING, GATE_ON_STRING.toLowerCase(), false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(GATE_STATE_STRING, GATE_STATE_STRING.toLowerCase(), juce::StringArray{ BASIC_GATE_STRING, RMS_GATE_STRING, ATTACK_HOLD_RELEASE_GATE_STRING }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(ATTACK_STRING, ATTACK_STRING.toLowerCase(), juce::NormalisableRange<float>(ATTACK_MIN, 0.15, 0.001, 1.f), ATTACK_MIN));
    layout.add(std::make_unique<juce::AudioParameterFloat>(RELEASE_STRING, RELEASE_STRING.toLowerCase(), juce::NormalisableRange<float>(RELEASE_MIN, 3, 0.001, 1.f), RELEASE_MIN));
    layout.add(std::make_unique<juce::AudioParameterFloat>(HOLD_STRING, HOLD_STRING.toLowerCase(), juce::NormalisableRange<float>(HOLD_MIN, 1.5, 0.001, 1.f), HOLD_MIN));

    layout.add(std::make_unique<juce::AudioParameterBool>(DISTORTION_ON_STRING, DISTORTION_ON_STRING.toLowerCase(), false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(DISTORTION_STATE_STRING, DISTORTION_STATE_STRING.toLowerCase(), 
    juce::StringArray {BIT_CRUSHER_STRING, WAVE_RECTIFIER_STRING, SOFT_CLIPPER_CUBIC_STRING, SOFT_CLIPPER_ARCTAN_STRING, SLEW_LIMITER_STRING }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(DISTORTION_STRING, DISTORTION_STRING.toLowerCase(), juce::NormalisableRange<float>(0, 1.f, 0.001, 1.f), 0.f));

    layout.add(std::make_unique<juce::AudioParameterBool>(FLANGER_ON_STRING, FLANGER_ON_STRING.toLowerCase(), false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(FLANGER_DELAY_STRING, FLANGER_DELAY_STRING.toLowerCase(), 0, DELAY_MAX, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(FLANGER_MIX_STRING, FLANGER_MIX_STRING.toLowerCase(), juce::NormalisableRange<float>(0, 100, 0.01f, 1), 0));

    return layout;

}
void BasicFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}


void BasicFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    if (*gateOnParam) {
        switch (int(*gateStateParam)) {
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
    if (*distortionOnParam) {
        switch (int(*distortionStateParam)) {
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
    if (*flangerOnParam) {
        processFlanger(buffer);
    }

}



void BasicFXAudioProcessor::processFlanger(juce::AudioBuffer<float>& buffer) {
    
    
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();
    float delayInSamples = *flangerDelayParam / float(sampleRate);
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < delayInSamples; sampleNum++) {
         
            float flangeSample = 1;

            float drySample = (1 - *flangerMixParam / 100) * channelData[sampleNum];
            float wetSample = ((*flangerMixParam) / 100) * flangeSample;
            channelData[sampleNum] = drySample + wetSample;
        }
    }
}

void BasicFXAudioProcessor::processDistortionWaveRectifier(juce::AudioBuffer<float>& buffer) {
    //at 0 behaves as a half-wave rectifier, at 1 behaves as a full-wave rectifier,
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            if (channelData[sampleNum] < 0) {
                channelData[sampleNum] *= *distortionParam * -1;
            }
        }
    }
}
float bitCrush(float sample, int bits) {
        
    float stepSize = 2.0f / (bits - 1);
    float crushedSample = round(sample / stepSize) * stepSize;
    return juce::jlimit(-1.0f, 1.0f, crushedSample);
}

void BasicFXAudioProcessor::processDistortionBitCrusher(juce::AudioBuffer<float>& buffer) {
    //starts at 100 bits and goes down to 1 bit 
    int numBits = int((1-*distortionParam) * 100); 
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            if (*distortionParam != 0) { 
                channelData[sampleNum] = bitCrush(channelData[sampleNum], numBits);
            }
        }
    }
}
void BasicFXAudioProcessor::processDistortionSoftClipperCubic(juce::AudioBuffer<float>& buffer) {
    //at distortion = 0 acts as a linear function (signal passes through normally), at distortion = 1 acts as a cubic function
    
    float constant = (1.f / 3.f); //TODO make this a knob?
    //constant = 1; //for testing
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            channelData[sampleNum] = channelData[sampleNum] + (*distortionParam) * constant * pow(channelData[sampleNum], 3.f);
        }
    }
}
void BasicFXAudioProcessor::processDistortionSoftClipperArcTan(juce::AudioBuffer<float>& buffer) {
    // applies (2/pi) * arctan(alpha * x) where alpha is controlled by the distortion knob

    float alphaMultiplier = 100; //TODO make this a knob?
    float alpha = *distortionParam * alphaMultiplier;
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; channel++) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
            channelData[sampleNum] = (2 / juce::MathConstants<float>::pi) * std::atan( alpha * channelData[sampleNum]);
            
        }
    }
}
void BasicFXAudioProcessor::processDistortionSlewLimiter(juce::AudioBuffer<float>& buffer) {
    // limits rate of change between samples 
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    float maxDelta = 1 - *distortionParam;
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
                currSample = prevSample + juce::jlimit(-maxDelta, maxDelta, maxDelta);
            }
            //TODO: add fall case
            channelData[sampleNum] = currSample;
        }
        lastSampleFromPrevBuffer[channel] = channelData[numSamples - 1];
    }
}

float calculateRMS(const float* samples, int numSamples) {
    float sum = 0;
    for (int i = 0; i < numSamples; ++i) {
        sum += samples[i] * samples[i];  
    }
    return std::sqrt(sum / numSamples);
}
float calculateRMSAcrossChannels(juce::AudioBuffer<float>& buffer) {
    int numChannels = buffer.getNumChannels();
    int numSamples = buffer.getNumSamples();
    float sum = 0;

    for (int channel = 0; channel < numChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        for (int i = 0; i < numSamples; ++i) {
            sum += channelData[i] * channelData[i];
        }
    }
    float rms = std::sqrt(sum / (numSamples * numChannels));
    return rms;
}

void BasicFXAudioProcessor::processGateSimple(juce::AudioBuffer<float>& buffer) {
    auto totalNumInputChannels = getTotalNumInputChannels();
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

void BasicFXAudioProcessor::processGateMedium(juce::AudioBuffer<float>& buffer) {
    auto totalNumInputChannels = getTotalNumInputChannels();
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

void BasicFXAudioProcessor::processGateAdvanced(juce::AudioBuffer<float>& buffer) {


    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();
    float attack = *attackParam;
    float release = *releaseParam;
    float hold = *holdParam;

    //TODO: handle changes in params from UI where sampleCounters become greater than the samplesPerWindow, causing extreme loudness
    /*
    if (attackActive && cachedAttack != attack) {
        attackSampleCounter = int(attackSampleCounter * cachedAttack / attack);
        cachedAttack = attack;
    }
    if (holdActive && cachedHold != hold) {
        holdSampleCounter = int(holdSampleCounter * cachedHold / hold);
        cachedHold = hold;
    }
    if (releaseActive && cachedRelease != release) {
        releaseSampleCounter = int(releaseSampleCounter * cachedRelease / release);
        cachedRelease = release;
    }
    */
    int samplesPerAttackWindow = attack * sampleRate; 
    int samplesPerReleaseWindow = release * sampleRate;
    int samplesPerHoldWindow = hold * sampleRate;


    float rms = calculateRMSAcrossChannels(buffer);
    bool gateCondition = (rms < *thresholdParam && rms != 0); //TODO: choose a better condition
    if (!attackActive && !holdActive && !releaseActive && gateCondition) {
        //gate is closed, check incoming values and set attack based on some trigger condition
        attackActive = true;
        attackStartRatio = 1.0f; 
        attackSampleCounter = samplesPerAttackWindow;
        
    }
    if (gateCondition) {
        // handle if the is retriggered? do I need this?
        /*
        if (attackActive) {
            //extend the next hold phase 
            if (retriggerHoldSampleCounter == 0) {
                retriggerHoldSampleCounter = std::max(0, samplesPerAttackWindow - attackSampleCounter);
            }
        }
        else if (holdActive) {
            //extend the current hold phase
                if (holdSampleCounter < samplesPerHoldWindow) {
                    holdSampleCounter = samplesPerHoldWindow;
                }
        }
        if (releaseActive) {
            // start a new attack phase based on where the release window is
            
            releaseActive = false;
            float ratio = 1.0f - float(releaseSampleCounter) / float(samplesPerReleaseWindow);
            attackStartRatio = 1- std::pow(ratio, 2.0f);
            attackActive = true;
            attackSampleCounter = int(ratio *samplesPerAttackWindow);
            
        }
        */
    }
       
    float* channelData[MAX_INPUT_CHANNELS];
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        channelData[channel] = buffer.getWritePointer(channel);
    }

    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        if (attackActive) {
            float ratio = 1.0f - float(attackSampleCounter) / float(samplesPerAttackWindow); 
            float smoothedRatio =  (1.0f - std::pow(ratio, 2.0f));
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] *= attackStartRatio * smoothedRatio;
            }
            attackSampleCounter--;

            if (attackSampleCounter <= 0) {
                attackActive = false;
                holdActive = true;
                holdSampleCounter = samplesPerHoldWindow + retriggerHoldSampleCounter;
                retriggerHoldSampleCounter = 0;
            }
        }
        if (holdActive) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] = 0;
            }
            holdSampleCounter--;
            if (holdSampleCounter <= 0) {
                holdActive = false;
                releaseActive = true;
                releaseSampleCounter = samplesPerReleaseWindow;
            }
        }
        if (releaseActive) {
            float ratio = 1.0f - float(releaseSampleCounter) / float(samplesPerReleaseWindow);
            float smoothedRatio = std::pow(ratio, 2.0f); 
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                channelData[channel][sampleNum] *= smoothedRatio;
            }
            releaseSampleCounter--;
            if (releaseSampleCounter <= 0) {
                releaseActive = false;
            }
        }
        else {
            //sample unchanged
        }
    }
            
    
}


//==============================================================================
const juce::String BasicFXAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicFXAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicFXAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicFXAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicFXAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicFXAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int BasicFXAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicFXAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BasicFXAudioProcessor::getProgramName (int index)
{
    return {};
}

void BasicFXAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================


void BasicFXAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicFXAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



//==============================================================================
bool BasicFXAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BasicFXAudioProcessor::createEditor()
{
    return new BasicFXAudioProcessorEditor (*this, apvts);
}

//==============================================================================
void BasicFXAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    juce::MemoryOutputStream mos(destData, true);
    apvts.state.writeToStream(mos);
}

void BasicFXAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.

    auto tree = juce::ValueTree::readFromData(data, sizeInBytes);
    if (tree.isValid()) {
        apvts.replaceState(tree);
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicFXAudioProcessor();
}
