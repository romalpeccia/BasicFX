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
#include "Components/EmptyComponent.h"
#include "Utilities.h"

class SwappableComponentManager : public juce::Component, public juce::ActionListener, public juce::ActionBroadcaster {
    public:

        SwappableComponentManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts);

        void resized() override;

        void actionListenerCallback(const juce::String& message) override;
        void handleDraggedComponent(SwappableComponent& draggedComp);
        void swapComponents(SwappableComponent& draggedComp, SwappableComponent& otherComp);
        int findComponentIndex(const SwappableComponent& component);
        std::vector<SwappableComponent*> getComponentList();
        void initializeComponents();

    private:
        std::vector<std::unique_ptr<SwappableComponent>> swappableComponents; // contains pointers to all of our components. Unique pointers allow us to change what they point to delete the old component and assign a new one
        BasicFXAudioProcessor& audioProcessor;
        juce::AudioProcessorValueTreeState& apvts;

};