/*
  ==============================================================================

    SwappableComponentManager.h
    Created: 24 Apr 2025 11:59:21am
    Author:  romal

  ==============================================================================
*/

#pragma once
#include<JuceHeader.h>
#include "PluginProcessor.h"
#include "Components/SwappableComponent.h"
#include "Components/GateComponent.h"
#include "Components/DistortionComponent.h"
#include "Components/FlangerComponent.h"
#include "Components/EQComponent.h"
#include "Components/EmptyComponent.h"
#include "Utilities.h"

class SwappableComponentManager : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster {
    public:

        SwappableComponentManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts);
        void initializeComponents();

        void resized() override;
        void actionListenerCallback(const juce::String& message) override;
        void handleDraggedComponent(SwappableComponent& draggedComp);

        void swapComponents(SwappableComponent& draggedComp, SwappableComponent& otherComp); 
        void swapProcessorParams(SwappableComponent& draggedComponent, SwappableComponent& otherComponent); //called by swapComponents
        
        //helper functions for swapProcessorParams
        void swapGateParams(GateProcessor* a, GateProcessor* b);
        void swapDistortionParams(DistortionProcessor* a, DistortionProcessor* b);
        void swapFlangerParams(FlangerProcessor* a, FlangerProcessor* b);
        void swapEQParams(EQProcessor* a, EQProcessor* b);

        //TODO make these function calls coorespond to default values
        void moveDistortionParams(DistortionProcessor* distortionProcessor, int index);
        void moveGateParams(GateProcessor* gateProcessor, int index);
        void moveFlangerParams(FlangerProcessor* flangerProcessor, int index);
        void moveEQParams(EQProcessor* eqProcessor, int index);

        int getComponentIndex(const SwappableComponent& component);
        std::vector<SwappableComponent*> getComponentList();


    private:
        std::vector<std::unique_ptr<SwappableComponent>> swappableComponents; // contains pointers to all of our components. Unique pointers allow us to change what they point to delete the old component and assign a new one
        BasicFXAudioProcessor& audioProcessor;
        juce::AudioProcessorValueTreeState& apvts;

};