/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "MultiBandSignalChainProcessor.h"
#include "MultiBandSignalChainManager.h"

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

}

BasicFXAudioProcessor::~BasicFXAudioProcessor()
{
}




void BasicFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    if (signalChainComponents != nullptr ) {
        if (auto* signalChainProcessors = signalChainComponents->getMultiBandSignalChainProcessors()) { //TODO this seems janky
            if (signalChainProcessors != nullptr) {
                signalChainProcessors->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumInputChannels());
            }
        }
    }
    visualizerIncomingProcessor.prepareToPlay(sampleRate, getTotalNumInputChannels());
    visualizerOutgoingProcessor.prepareToPlay(sampleRate, getTotalNumInputChannels());
}


void BasicFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals; //prevents numbers from blowing up due to weird floating point math
    //clear unused channels
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples); // clear unused buffers


    dbMeterIncomingProcessor.processBlock(buffer);
    visualizerIncomingProcessor.processBlock(buffer);
    
    if (signalChainComponents != nullptr) {
        if (auto* signalChainProcessors = signalChainComponents->getMultiBandSignalChainProcessors()) {
            if (signalChainProcessors != nullptr ){ // TODO uncesary?
                signalChainProcessors->processBlock(buffer);
            }
        }
    }
    dbMeterOutgoingProcessor.processBlock(buffer);
    visualizerOutgoingProcessor.processBlock(buffer);
}

void BasicFXAudioProcessor::actionListenerCallback(const juce::String& message) {
    
    if (message.startsWith("SWAPPED_") || message.startsWith("CREATECOMPONENT") || message.startsWith("DELETECOMPONENT"))
    {   //called by SwappableComponentManager::swapComponents, SwappableComponent xButton, and menu onClick methods
        //rebuild the signal chain
        if (signalChainComponents != nullptr) {
            if (auto* signalChainProcessors = signalChainComponents->getMultiBandSignalChainProcessors()) {
                if (signalChainProcessors != nullptr) {
                    signalChainProcessors->rebuildSignalChains();
                }
                
            }
        }
    }
    
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicFXAudioProcessor::createParameterLayout() {
    //Creates all the parameters that change based on the user input and returns them in a AudioProcessorValueTreeState::ParameterLayout object

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int band = 0; band < MAX_BANDS; band++) {
        for (int comp = 0; comp < MAX_COMPONENTS; comp++) {
            addGateParametersToLayout(layout, band, comp);
            addDistortionParametersToLayout(layout, band, comp);
            addFlangerParametersToLayout(layout, band, comp);
            addEQParametersToLayout(layout, band, comp);
            /*
            layout.add(std::make_unique<juce::AudioParameterChoice>(makeMultibandParamID(SWAPPABLE_COMPONENT_TYPE_STRING, i), makeMultibandParamName(SWAPPABLE_COMPONENT_TYPE_STRING, i),
                juce::StringArray{ "EMPTY", "GATE", "DISTORTION", "FLANGER", "EQ"}, 0)); //TODO: this doesnt do anything yet but will be important when loading/saving component states
                */
        }
    }

    layout.add(std::make_unique<juce::AudioParameterInt>(DB_METER_RATE_STRING, DB_METER_RATE_STRING.toLowerCase(), 1, 500, 33));

    return layout;
}

void BasicFXAudioProcessor::addGateParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int bandIndex, int componentIndex) {
    if (bandIndex >= 0) {
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(THRESHOLD_STRING, bandIndex, componentIndex), makeMultibandParamName(THRESHOLD_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
        layout.add(std::make_unique<juce::AudioParameterBool>(makeMultibandParamID(GATE_ON_STRING, bandIndex, componentIndex), makeMultibandParamName(GATE_ON_STRING, bandIndex, componentIndex), false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeMultibandParamID(GATE_STATE_STRING, bandIndex, componentIndex), makeMultibandParamName(GATE_STATE_STRING, bandIndex, componentIndex), juce::StringArray{ BASIC_GATE_STRING, RMS_GATE_STRING, ATTACK_HOLD_RELEASE_GATE_STRING }, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(ATTACK_STRING, bandIndex, componentIndex), makeMultibandParamName(ATTACK_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(ATTACK_MIN, 0.15f, 0.001f, 1.f), ATTACK_MIN));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(RELEASE_STRING, bandIndex, componentIndex), makeMultibandParamName(RELEASE_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(RELEASE_MIN, 3.0f, 0.001f, 1.f), RELEASE_MIN));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(HOLD_STRING, bandIndex, componentIndex), makeMultibandParamName(HOLD_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(HOLD_MIN, 1.5f, 0.001f, 1.f), HOLD_MIN));
    }
    else {
                /*
         layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(THRESHOLD_STRING, i), makeName(THRESHOLD_STRING, i), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
         layout.add(std::make_unique<juce::AudioParameterBool>(makeID(GATE_ON_STRING, i), makeName(GATE_ON_STRING, i), false));
         layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(GATE_STATE_STRING, i), makeName(GATE_STATE_STRING, i), juce::StringArray{ BASIC_GATE_STRING, RMS_GATE_STRING, ATTACK_HOLD_RELEASE_GATE_STRING }, 0));
         layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(ATTACK_STRING, i), makeName(ATTACK_STRING, i), juce::NormalisableRange<float>(ATTACK_MIN, 0.15f, 0.001f, 1.f), ATTACK_MIN));
         layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(RELEASE_STRING, i), makeName(RELEASE_STRING, i), juce::NormalisableRange<float>(RELEASE_MIN, 3.0f, 0.001f, 1.f), RELEASE_MIN));
         layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(HOLD_STRING, i), makeName(HOLD_STRING, i), juce::NormalisableRange<float>(HOLD_MIN, 1.5f, 0.001f, 1.f), HOLD_MIN));
         */
    }
 
}

void BasicFXAudioProcessor::addDistortionParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int bandIndex, int componentIndex) {
    if (bandIndex >= 0) {
        layout.add(std::make_unique<juce::AudioParameterBool>(makeMultibandParamID(DISTORTION_ON_STRING, bandIndex, componentIndex), makeMultibandParamName(DISTORTION_ON_STRING, bandIndex, componentIndex), false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeMultibandParamID(DISTORTION_TYPE_STRING, bandIndex, componentIndex), makeMultibandParamName(DISTORTION_TYPE_STRING, bandIndex, componentIndex),
            juce::StringArray{ BIT_CRUSHER_STRING, WAVE_RECTIFIER_STRING, SOFT_CLIPPER_CUBIC_STRING, SOFT_CLIPPER_ARCTAN_STRING, SLEW_LIMITER_STRING }, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(DISTORTION_AMOUNT_STRING, bandIndex, componentIndex), makeMultibandParamName(DISTORTION_AMOUNT_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
    }
    else {
        /*
        layout.add(std::make_unique<juce::AudioParameterBool>(makeID(DISTORTION_ON_STRING, i), makeName(DISTORTION_ON_STRING, i), false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(DISTORTION_TYPE_STRING, i), makeName(DISTORTION_TYPE_STRING, i),
            juce::StringArray{ BIT_CRUSHER_STRING, WAVE_RECTIFIER_STRING, SOFT_CLIPPER_CUBIC_STRING, SOFT_CLIPPER_ARCTAN_STRING, SLEW_LIMITER_STRING }, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(DISTORTION_AMOUNT_STRING, i), makeName(DISTORTION_AMOUNT_STRING, i), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
        */
    }
}

void BasicFXAudioProcessor::addFlangerParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int bandIndex, int componentIndex) {
    if (bandIndex >= 0) {
        layout.add(std::make_unique<juce::AudioParameterBool>(makeMultibandParamID(FLANGER_ON_STRING, bandIndex, componentIndex), makeMultibandParamName(FLANGER_ON_STRING, bandIndex, componentIndex), false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(FLANGER_DELAY_STRING, bandIndex, componentIndex), makeMultibandParamName(FLANGER_DELAY_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0, DELAY_MAX, 0.01f, 1.f), 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(FLANGER_MIX_STRING, bandIndex, componentIndex), makeMultibandParamName(FLANGER_MIX_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0, 100, 0.01f, 1.f), 0));
    }
    else {
        /*
        layout.add(std::make_unique<juce::AudioParameterBool>(makeID(FLANGER_ON_STRING, i), makeName(FLANGER_ON_STRING, i), false));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(FLANGER_DELAY_STRING, i), makeName(FLANGER_DELAY_STRING, i), juce::NormalisableRange<float>(0, DELAY_MAX, 0.01f, 1.f), 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(FLANGER_MIX_STRING, i), makeName(FLANGER_MIX_STRING, i), juce::NormalisableRange<float>(0, 100, 0.01f, 1.f), 0));
        */
    }
}

void BasicFXAudioProcessor::addEQParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int bandIndex, int componentIndex) {
    
    if (bandIndex >= 0) {
        layout.add(std::make_unique<juce::AudioParameterBool>(makeMultibandParamID(EQ_ON_STRING, bandIndex, componentIndex), makeMultibandParamName(EQ_ON_STRING, bandIndex, componentIndex), false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeMultibandParamID(EQ_TYPE_STRING, bandIndex, componentIndex), makeMultibandParamName(EQ_TYPE_STRING, bandIndex, componentIndex),
            juce::StringArray{ LOW_PASS_STRING, BAND_PASS_STRING, HIGH_PASS_STRING }, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(EQ_LOW_FREQUENCY_STRING, bandIndex, componentIndex), makeMultibandParamName(EQ_LOW_FREQUENCY_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0.1, 20000.f, 1.f, 1.f), 0.f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeMultibandParamID(EQ_HIGH_FREQUENCY_STRING, bandIndex, componentIndex), makeMultibandParamName(EQ_HIGH_FREQUENCY_STRING, bandIndex, componentIndex), juce::NormalisableRange<float>(0.1, 20000.f, 1.f, 1.f), 0.f));
    }
    else {/*
        layout.add(std::make_unique<juce::AudioParameterBool>(makeID(EQ_ON_STRING, i), makeName(EQ_ON_STRING, i), false));
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(EQ_TYPE_STRING, i), makeName(EQ_TYPE_STRING, i),
            juce::StringArray{ LOW_PASS_STRING, BAND_PASS_STRING, HIGH_PASS_STRING}, 0));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(EQ_LOW_FREQUENCY_STRING, i), makeName(EQ_LOW_FREQUENCY_STRING, i), juce::NormalisableRange<float>(0.1, 20000.f, 1.f, 1.f), 0.f));
        layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(EQ_HIGH_FREQUENCY_STRING, i), makeName(EQ_HIGH_FREQUENCY_STRING, i), juce::NormalisableRange<float>(0.1, 20000.f, 1.f, 1.f), 0.f));
        */
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
    //return new GenericAudioProcessorEditor(*this);
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
    // whose contents will have been created by the getOnStateInformation() call.

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
