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
    layout.add(std::make_unique<juce::AudioParameterFloat>(ATTACK_STRING, ATTACK_STRING.toLowerCase(), juce::NormalisableRange<float>(0.00002, 0.15, 0.001, 1.f), 0.00002));
    layout.add(std::make_unique<juce::AudioParameterFloat>(RELEASE_STRING, RELEASE_STRING.toLowerCase(), juce::NormalisableRange<float>(0.0001, 3, 0.001, 1.f), 0.0001));
    layout.add(std::make_unique<juce::AudioParameterFloat>(HOLD_STRING, HOLD_STRING.toLowerCase(), juce::NormalisableRange<float>(0.001, 1.5, 0.001, 1.f), 0.001));

    layout.add(std::make_unique<juce::AudioParameterBool>(DISTORTION_ON_STRING, DISTORTION_ON_STRING.toLowerCase(), false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(DISTORTION_STATE_STRING, DISTORTION_STATE_STRING.toLowerCase(), juce::StringArray {BIT_CRUSHER_STRING, WAVE_RECTIFIER_STRING, SOFT_CLIPPER_STRING, SLEW_LIMITER_STRING }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(DISTORTION_STRING, DISTORTION_STRING.toLowerCase(), juce::NormalisableRange<float>(0, 1.f, 0.001, 1.f), 0.f));
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

    DBG(*gateStateParam);
    if (*gateOnParam == true) {
        if (*gateStateParam == 0) {
            processGateSimple(buffer);
        }
        else if (*gateStateParam == 1) {
            processGateMedium(buffer);
        }
        else if (*gateStateParam == 2) {
            processGateAdvanced(buffer);
        }
    }
   
    if (*distortionOnParam == true) {
        if (*distortionStateParam == 0) {
            processDistortionWaveRectifier(buffer);
        }
        else if (*distortionStateParam == 1) {

        }
        else if (*distortionStateParam == 2) {

        }
        else if (*distortionStateParam == 3) {

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
void BasicFXAudioProcessor::processDistortionBitCrusher(juce::AudioBuffer<float>& buffer) {

}
void BasicFXAudioProcessor::processDistortionSoftClipper(juce::AudioBuffer<float>& buffer) {

}
void BasicFXAudioProcessor::processDistortionSlewLimiter(juce::AudioBuffer<float>& buffer) {

}

float calculateRMS(const float* samples, int numSamples) {
    float sum = 0.0;
    for (int i = 0; i < numSamples; ++i) {
        sum += samples[i] * samples[i];  
    }
    return std::sqrt(sum / numSamples);
}

void BasicFXAudioProcessor::processGateSimple(juce::AudioBuffer<float>& buffer) {
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        bool triggerGate = false;
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            if (abs(channelData[sampleNum]) < *thresholdParam) {
                triggerGate = true;
                break;
            }
        }
        if (triggerGate == true) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                channelData[sampleNum] = 0;
            }
        }
    }
}

void BasicFXAudioProcessor::processGateMedium(juce::AudioBuffer<float>& buffer) {
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();

    bool triggerGate = false;
    for (int channel = 0; channel < totalNumInputChannels; ++channel) {
        auto* channelData = buffer.getWritePointer(channel);
        if (calculateRMS(channelData, numSamples) < *thresholdParam) {
            triggerGate = true;
            break;
        }
    }
    if (triggerGate == true) {

        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
                channelData[sampleNum] = 0;
            }
        }
    }
}

void BasicFXAudioProcessor::processGateAdvanced(juce::AudioBuffer<float>& buffer) {
    //TODO: try rewriting this with better handling of state. 
    auto totalNumInputChannels = getTotalNumInputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();
    float attack = *attackParam;
    float release = *releaseParam;
    float hold = *holdParam;
    int samplesPerAttackWindow = attack * sampleRate;
    int samplesPerReleaseWindow = release * sampleRate;
    int samplesPerHoldWindow = hold * sampleRate;

    int attackSampleCounter = 0, releaseSampleCounter = 0, holdSampleCounter = 0;
    bool attackActive = false, releaseActive = false, holdActive = false;

    if (attackActiveFromPrevBuffer) {
        attackActive = true;
        attackSampleCounter = numSamplesFromPrevAttack;
    }
    else if (releaseActiveFromPrevBuffer) {
        releaseActive = true;
        releaseSampleCounter = numSamplesFromPrevRelease;
    }

    if (holdActiveFromPrevBuffer) {
        holdActive = true;
        holdSampleCounter = numSamplesFromPrevHold;
    }

    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {

        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel); //TODO make this more efficient?
            float rms = calculateRMS(channelData, numSamples);
            if (rms < *thresholdParam && rms != 0) {
                if (!releaseActive && !holdActive) {

                    attackSampleCounter = samplesPerAttackWindow;
                }
                else {
                    holdActive = true;
                    holdSampleCounter = samplesPerHoldWindow;
                }
                attackActive = true;
                break;
            }
        }

        //holdActive = false; //for debugging
        if (attackActive == true) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                float ratio = float(attackSampleCounter) / float(samplesPerAttackWindow);
                channelData[sampleNum] *= ratio;
            }

            attackSampleCounter--;

            if (holdActive == true) {
                holdSampleCounter--;
            }
            else {

            }

            if (attackSampleCounter <= 0) {
                if (holdActive == true) {
                    if (holdSampleCounter <= 0) {
                        holdActive = false;
                    }
                    attackSampleCounter = 0;//this sets the ratio to multiply the samples by to 0 while hold is active
                }
                else {
                    attackActive = false;
                    attackActiveFromPrevBuffer = false;

                    releaseActive = true;
                    releaseSampleCounter = samplesPerReleaseWindow;
                }

            }
        }

        if (releaseActive == true && holdActive == false) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);

                float ratio = 1 - (float(releaseSampleCounter) / float(samplesPerReleaseWindow));
                channelData[sampleNum] *= ratio;
            }
            releaseSampleCounter--;

            if (releaseSampleCounter <= 0) {
                releaseActive = false;
                releaseActiveFromPrevBuffer = false;
            }
        }
        else if (holdActive == true) {
            holdSampleCounter--;
            if (holdSampleCounter <= 0) {
                holdActive = false;
            }
            else {
                for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                    auto* channelData = buffer.getWritePointer(channel);
                    channelData[sampleNum] = 0;
                }
            }

        }

        if (releaseActive == true && attackActive == true) {
            DBG("ATTACK/RELEASE ERROR");
        }
    }

    if (attackSampleCounter > 0) {
        numSamplesFromPrevAttack = attackSampleCounter;
        attackActiveFromPrevBuffer = true;
    }
    else if (releaseSampleCounter > 0) {
        numSamplesFromPrevAttack = 0;
        attackActiveFromPrevBuffer = false;

        numSamplesFromPrevRelease = releaseSampleCounter;
        releaseActiveFromPrevBuffer = true;
    }
    else {
        releaseActiveFromPrevBuffer = false;
        numSamplesFromPrevRelease = 0;
    }
    if (holdSampleCounter > 0) {
        numSamplesFromPrevHold = holdSampleCounter;
        holdActiveFromPrevBuffer = true;
    }

    float gateValue = *thresholdParam; // NOTE: why cant i just put *thresholdParam in the function? weird
    auto db = juce::Decibels::toString(juce::Decibels::gainToDecibels(gateValue), 2, -100.f, false);
    //DBG(db);
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
