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
    gateParam = apvts.getRawParameterValue(GATE_STRING);
}

BasicFXAudioProcessor::~BasicFXAudioProcessor()
{
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicFXAudioProcessor::createParameterLayout() {
    //Creates all the parameters that change based on the user input and returns them in a AudioProcessorValueTreeState::ParameterLayout object

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterFloat>(GATE_STRING, GATE_STRING.toLowerCase(), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));

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

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();



    //noise gate simple
    /*
    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
        bool triggerGate = false;
        for (int channel = 0; channel < totalNumInputChannels; ++channel) {
            auto* channelData = buffer.getWritePointer(channel);
            if (abs(channelData[sampleNum]) < *gateParam) {
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
    }*/
    
    /**/
    float attack = 0.001;
    float release = 0.2;
    float hold = 0.02;
    int samplesPerAttackWindow = attack * sampleRate;
    int samplesPerReleaseWindow = release * sampleRate;
    int samplesPerHoldWindow = hold * sampleRate;

    int attackSampleCounter = 0, releaseSampleCounter = 0;
    bool attackActive = false, releaseActive = false;
    
    if (attackOpenFromPrevBuffer) {
        attackActive = true;
        attackSampleCounter = numSamplesFromPrevAttack;
       // DBG("ATTACK");
    }
    else if (releaseOpenFromPrevBuffer) {
        releaseActive = true;
        releaseSampleCounter = numSamplesFromPrevRelease;
       // DBG("RELEASE");
    }
 
    for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {

        if (!attackActive && !releaseActive) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel); //TODO make this more efficient?
                if (fabs(channelData[sampleNum]) < *gateParam && channelData[sampleNum] != 0) {
                    attackActive = true;
                    attackSampleCounter = samplesPerAttackWindow;
                    break;
                }
            }
        }

        if (attackActive == true) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                float ratio = float(attackSampleCounter) / float(samplesPerAttackWindow);
                channelData[sampleNum] *= ratio;
            }

            attackSampleCounter--;

            if (attackSampleCounter <= 0) {
                attackActive = false;
                attackOpenFromPrevBuffer = false;

                releaseActive = true;
                releaseSampleCounter = samplesPerReleaseWindow;
            }
        }

        if (releaseActive == true) {
            for (int channel = 0; channel < totalNumInputChannels; ++channel) {
                auto* channelData = buffer.getWritePointer(channel);
                float ratio = 1 - (float(releaseSampleCounter) / float(samplesPerReleaseWindow));
                channelData[sampleNum] *= ratio;
            }
            releaseSampleCounter--;


            if (releaseSampleCounter <= 0) {
                releaseActive = false;
                releaseOpenFromPrevBuffer = false;
            }
        }

        if (releaseActive == true && attackActive == true) {
            DBG("ATTACK");
            DBG(attackSampleCounter); //TODO: throw exception
            DBG("RELEASE");
            DBG(releaseSampleCounter);
        }
    }
    
    if (attackSampleCounter > 0) {
        numSamplesFromPrevAttack = attackSampleCounter;
        attackOpenFromPrevBuffer = true;
    }
    else if (releaseSampleCounter > 0) {
        numSamplesFromPrevAttack = 0;
        attackOpenFromPrevBuffer = false;

        numSamplesFromPrevRelease = releaseSampleCounter;
        releaseOpenFromPrevBuffer = true;
    }
    else {
        releaseOpenFromPrevBuffer = false;
        numSamplesFromPrevRelease = 0;
    }
    

    float gateValue = *gateParam; // NOTE: why cant i just put *gateParam in the function? weird
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
