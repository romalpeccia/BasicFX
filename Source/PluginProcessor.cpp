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



}

BasicFXAudioProcessor::~BasicFXAudioProcessor()
{
}




void BasicFXAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{

    for (auto* processor : signalChain)
    {
        processor->prepareToPlay(sampleRate, getTotalNumInputChannels());
    }
    visualizerIncomingProcessor.prepareToPlay(sampleRate, getTotalNumInputChannels());
    visualizerOutgoingProcessor.prepareToPlay(sampleRate, getTotalNumInputChannels());
}


void BasicFXAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals; //prevents numbers from 
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    int numSamples = buffer.getNumSamples();
    int sampleRate = getSampleRate();

    //clear unused channels
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, numSamples);

    dbMeterIncomingProcessor.processBlock(buffer);
    visualizerIncomingProcessor.processBlock(buffer);

    for (auto* processor : signalChain)
    {
        if (processor != nullptr)
            processor->processBlock(buffer);
    }
    dbMeterOutgoingProcessor.processBlock(buffer);
    visualizerOutgoingProcessor.processBlock(buffer);
}

void BasicFXAudioProcessor::actionListenerCallback(const juce::String& message) {
    
    auto& componentList = swappableComponentManager->getComponentList();
    if (message.startsWith("SWAPPED_"))
    {   //called by SwappableComponentManager::swapComponents


        //rebuild the signal chain
        signalChain.clear();
        for (auto* comp : componentList)
        {
            if (comp->getProcessor() != nullptr)
                signalChain.push_back(comp->getProcessor());
        }
        
    }
    else if (message.startsWith("CREATECOMPONENT")) {
        //called by EmptyComponent.menu.onClick
        signalChain.clear();
        for (auto* comp : componentList)
        {
            if (comp->getProcessor() != nullptr)
                signalChain.push_back(comp->getProcessor());
        }
    }
    
}

juce::AudioProcessorValueTreeState::ParameterLayout BasicFXAudioProcessor::createParameterLayout() {
    //Creates all the parameters that change based on the user input and returns them in a AudioProcessorValueTreeState::ParameterLayout object

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    for (int i = 0; i < MAX_COMPONENTS; ++i) {
        addGateParametersToLayout(layout, i);
        addDistortionParametersToLayout(layout, i);
        addFlangerParametersToLayout(layout, i);
        layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(SWAPPABLE_COMPONENT_TYPE_STRING, i), makeName(SWAPPABLE_COMPONENT_TYPE_STRING, i),
            juce::StringArray{ "EMPTY", "GATE", "DISTORTION", "FLANGER" }, 0)); //TODO: this doesnt do anything yet but will be important when loading/saving component states
    }

    layout.add(std::make_unique<juce::AudioParameterInt>(DB_METER_RATE_STRING, DB_METER_RATE_STRING.toLowerCase(), 1, 500, 33));

    return layout;

}

void BasicFXAudioProcessor::addGateParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int i) {
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(THRESHOLD_STRING, i), makeName(THRESHOLD_STRING, i), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
    layout.add(std::make_unique<juce::AudioParameterBool>(makeID(GATE_ON_STRING, i), makeName(GATE_ON_STRING, i), false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(GATE_STATE_STRING, i), makeName(GATE_STATE_STRING, i), juce::StringArray{ BASIC_GATE_STRING, RMS_GATE_STRING, ATTACK_HOLD_RELEASE_GATE_STRING }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(ATTACK_STRING, i), makeName(ATTACK_STRING, i), juce::NormalisableRange<float>(ATTACK_MIN, 0.15f, 0.001f, 1.f), ATTACK_MIN));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(RELEASE_STRING, i), makeName(RELEASE_STRING, i), juce::NormalisableRange<float>(RELEASE_MIN, 3.0f, 0.001f, 1.f), RELEASE_MIN));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(HOLD_STRING, i), makeName(HOLD_STRING, i), juce::NormalisableRange<float>(HOLD_MIN, 1.5f, 0.001f, 1.f), HOLD_MIN));
}

void BasicFXAudioProcessor::addDistortionParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int i) {
    layout.add(std::make_unique<juce::AudioParameterBool>(makeID(DISTORTION_ON_STRING, i), makeName(DISTORTION_ON_STRING, i), false));
    layout.add(std::make_unique<juce::AudioParameterChoice>(makeID(DISTORTION_TYPE_STRING, i), makeName(DISTORTION_TYPE_STRING, i),
        juce::StringArray{ BIT_CRUSHER_STRING, WAVE_RECTIFIER_STRING, SOFT_CLIPPER_CUBIC_STRING, SOFT_CLIPPER_ARCTAN_STRING, SLEW_LIMITER_STRING }, 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(DISTORTION_AMOUNT_STRING, i), makeName(DISTORTION_AMOUNT_STRING, i), juce::NormalisableRange<float>(0, 1.f, 0.001f, 1.f), 0.f));
}

void BasicFXAudioProcessor::addFlangerParametersToLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int i) {
    layout.add(std::make_unique<juce::AudioParameterBool>(makeID(FLANGER_ON_STRING, i), makeName(FLANGER_ON_STRING, i), false));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(FLANGER_DELAY_STRING, i), makeName(FLANGER_DELAY_STRING, i), juce::NormalisableRange<float>(0, DELAY_MAX, 0.01f, 1.f), 0));
    layout.add(std::make_unique<juce::AudioParameterFloat>(makeID(FLANGER_MIX_STRING, i), makeName(FLANGER_MIX_STRING, i), juce::NormalisableRange<float>(0, 100, 0.01f, 1.f), 0));
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
