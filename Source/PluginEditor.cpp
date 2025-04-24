/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicFXAudioProcessorEditor::BasicFXAudioProcessorEditor (BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState & _apvts)
    : AudioProcessorEditor (&p), audioProcessor (p), apvts(_apvts)
{

    for (auto* comp : getVisibleComps())
    {
        addAndMakeVisible(comp);
    }
    for (int i = 0; i < MAX_COMPONENTS; i++)
    {
        if (i % 4 == 0)
        {
            swappableComponents.push_back(std::make_unique<GateComponent>(apvts, audioProcessor.gateProcessors[i].get()));
        }
        else if (i % 4 == 1)
        {
            swappableComponents.push_back(std::make_unique<DistortionComponent>(apvts, audioProcessor.distortionProcessors[i].get()));
        }
        else if (i % 4 == 2)
        {
            swappableComponents.push_back(std::make_unique<FlangerComponent>(apvts, audioProcessor.flangerProcessors[i].get()));
        }
        else {
            swappableComponents.push_back(std::make_unique<EmptyComponent>(audioProcessor.emptyProcessor.get()));
            swappableComponents[i]->addActionListener(this);
        }
        addAndMakeVisible(swappableComponents.back().get());
    }
    setSize (1200, 800);
    setResizable(true, true);
}

BasicFXAudioProcessorEditor::~BasicFXAudioProcessorEditor()
{
}


//==============================================================================
void BasicFXAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

}

void BasicFXAudioProcessorEditor::resized()
{
    auto pluginBounds = getLocalBounds();

    auto FXBounds = pluginBounds.withTrimmedTop(pluginBounds.getHeight()*0.2);
    auto monitoringAreaBounds  = pluginBounds.withTrimmedBottom(pluginBounds.getHeight() * 0.8);

    auto visualizerBounds = monitoringAreaBounds.withTrimmedRight(monitoringAreaBounds.getWidth() * 0.1).withTrimmedLeft(monitoringAreaBounds.getWidth() * 0.1);
    auto incomingDBMeterBounds = monitoringAreaBounds.withTrimmedRight(monitoringAreaBounds.getWidth() * 0.9);
    auto outgoingDBMeterBounds = monitoringAreaBounds.withTrimmedLeft(monitoringAreaBounds.getWidth() * 0.9);
    visualizerComponent.setBounds(visualizerBounds);
    incomingDBMeterComponent.setBounds(incomingDBMeterBounds);
    outgoingDBMeterComponent.setBounds(outgoingDBMeterBounds);
    
    resizeSwappableComponentVector(FXBounds);
}

void BasicFXAudioProcessorEditor::resizeSwappableComponentVector(juce::Rectangle<int> bounds) {
    auto componentList = SwappableComponent::getSwappableComponents();
    if (!componentList.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() / componentList.size();
        float height = bounds.getHeight();
        //start at the corner of bounds and iterate. 
        for (auto* comp : componentList) {
            comp->setBounds(x, y, width, height);
            x += width;
        }
    }
}

std::vector<juce::Component*> BasicFXAudioProcessorEditor::getVisibleComps() {
    std::vector<juce::Component*> comps;

    comps.push_back(&visualizerComponent);
    comps.push_back(&incomingDBMeterComponent);
    comps.push_back(&outgoingDBMeterComponent);
    return comps;
}

void BasicFXAudioProcessorEditor::actionListenerCallback(const juce::String& message) {

    if (message.startsWith("CREATECOMPONENT")) {
        juce::StringArray tokens;
        tokens.addTokens(message, "_", "");
        DBG("MESSAGE FROM COMPONENT " <<message);
        if (tokens.size() >= 3)
        {
            int index = tokens[1].getIntValue();
            DBG("EMPTY COMPONENT triggered " << index);
            if (index < 0 || index >= swappableComponents.size())
                return;
            if (tokens[2] != "EMPTY"){
                //delete the EmptyComponent (by changing its pointer, it automatically deletes due to unique_ptr logic), and create a new one
                if (tokens[2] == "GATE") {
                    swappableComponents[index] = std::make_unique<GateComponent>(apvts, audioProcessor.gateProcessors[index].get(), index); 
                    addAndMakeVisible(swappableComponents[index].get());
                    resized();
                }
                else if (tokens[2] == "DISTORTION") {
                    swappableComponents[index] = std::make_unique<DistortionComponent>(apvts, audioProcessor.distortionProcessors[index].get(), index);
                    addAndMakeVisible(swappableComponents[index].get());
                    resized();
                }
                else if (tokens[2] == "FLANGER") {
                    swappableComponents[index] = std::make_unique<FlangerComponent>(apvts, audioProcessor.flangerProcessors[index].get(), index);
                    addAndMakeVisible(swappableComponents[index].get());
                    resized();
                }
            }
        }
    }
}