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
        SwappableComponent(std::unique_ptr<SwappableProcessor> processorPtr);

        SwappableComponent::~SwappableComponent();
        virtual void setComponentAttachments(int index) = 0;


        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        juce::Rectangle<int> getInitialBounds() { return initialBounds; }
        juce::Rectangle<int> getDraggedBounds() { return draggedBounds; }
        void setBounds(juce::Rectangle<int> bounds);
        void setBounds(int x, int y, int width, int height);
        int getAreaOverLapThreshold() { return areaOverlapThreshold; }
        void setAreaOverLapThreshold();

        void resized() override;

        SwappableProcessor* getProcessor() const { return processor.get(); }
        void setProcessor(std::unique_ptr<SwappableProcessor> _processor) { processor = std::move(_processor); }

        SwappableComponentManager* getManager() const;
        void setManager(SwappableComponentManager* _swappableComponentManager); 


    private:

        juce::ComponentDragger componentDragger; //adding this member allows us to drag the component using the mouseEvent that triggered it
        juce::Rectangle<int> initialBounds; //bounds of the component before it was dragged
        juce::Rectangle<int> draggedBounds; //bounds of the component while it is being dragged
        int areaOverlapThreshold = 0; //how much area the component has to share with the other component to trigger swapping them in void swapComponents(SwappableComponent* otherComp);
        
        juce::TextButton xButton{ "X" };

    protected: //accessible by derived classes but not external code
        std::unique_ptr<SwappableProcessor> processor;
        SwappableComponentManager* swappableComponentManager = nullptr;
};