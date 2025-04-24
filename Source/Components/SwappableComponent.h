/*
  ==============================================================================

    SwappableComponent.h
    Created: 14 Apr 2025 7:56:50pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "../Utilities.h"
#include "../Processors/SwappableProcessor.h"

class SwappableComponentManager; //forward declaration of SwappableComponentManager to avoid circular dependencies

class SwappableComponent : public juce::Component, public juce::ActionBroadcaster {
public:
    SwappableComponent() = delete;
    SwappableComponent(SwappableProcessor* processorPtr) : processor(processorPtr)
    {

    }
    SwappableComponent(SwappableProcessor* processorPtr, int index)
        : processor(processorPtr)
    {

    }
    SwappableComponent::~SwappableComponent();

    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;


    void setBounds(juce::Rectangle<int> bounds);
    void setBounds(int x, int y, int width, int height);

    SwappableProcessor* getProcessor() const { return processor; }
    void setProcessor(SwappableProcessor* _processor) { processor = _processor; }

    //    void swapComponents(SwappableComponent* otherComp);
    //int getIndexInComponentList();
    // std::vector<SwappableComponent*>& getSwappableComponents() { return swappableComponentList; }

    void setManager(SwappableComponentManager* _swappableComponentManager);
    SwappableComponentManager* getManager() const;

    int getAreaOverLapThreshold() { return areaOverlapThreshold; }
    juce::Rectangle<int> getInitialBounds() { return initialBounds; }
    juce::Rectangle<int> getDraggedBounds() { return draggedBounds; }
    private:
        void setAreaOverLapThreshold();

        juce::ComponentDragger componentDragger; //adding this member allows us to drag the component using the mouseEvent that triggered it
        juce::Rectangle<int> initialBounds; //bounds of the component before it was dragged
        juce::Rectangle<int> draggedBounds; //bounds of the component while it is being dragged
        int areaOverlapThreshold = 0; //how much area the component has to share with the other component to trigger swapping them in void swapComponents(SwappableComponent* otherComp);
        
        

    protected: //accessible by derived classes but not external code
        SwappableProcessor* processor = nullptr;
        SwappableComponentManager* swappableComponentManager = nullptr;
};