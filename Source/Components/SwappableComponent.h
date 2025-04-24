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
class SwappableComponent : public juce::Component, public juce::ActionBroadcaster {
    public:
        SwappableComponent() = delete;
        SwappableComponent(SwappableProcessor* processorPtr): processor(processorPtr)
        {
            swappableComponentList.push_back(this);
            int index = getIndexInComponentList();
            DBG("CREATED COMPONENT AT INDEX " << index);
        }
        SwappableComponent(SwappableProcessor* processorPtr, int index)
            : processor(processorPtr)
        {

            auto& list = getSwappableComponents();
            if (index >= 0 && index < list.size())
            {
                list[index] = this;
                DBG("CREATING COMPONENT AT INDEX " << index);
            }
            else {
                DBG("CREATED COMPONENT AT WRONG INDEX");
                list.push_back(this);
            }

        }
        SwappableComponent::~SwappableComponent();

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void swapComponents(SwappableComponent* otherComp);
        
        void setBounds(juce::Rectangle<int> bounds);
        void setBounds(int x, int y, int width, int height);
        void setAreaOverLapThreshold();
        static std::vector<SwappableComponent*>& getSwappableComponents() { return swappableComponentList; }
        SwappableProcessor* getProcessor() const { return processor; }
        void setProcessor(SwappableProcessor* _processor) { processor = _processor; }
        int getIndexInComponentList();

    private:
        inline static std::vector<SwappableComponent*> swappableComponentList; //static vector shared across all instances of swappable Components

        juce::ComponentDragger componentDragger; //adding this member allows us to drag the component using the mouseEvent that triggered it
        juce::Rectangle<int> initialBounds; //bounds of the component before it was dragged
        juce::Rectangle<int> draggedBounds; //bounds of the component while it is being dragged
        int area_overlap_threshold = 0; //how much area the component has to share with the other component to trigger swapping them in void swapComponents(SwappableComponent* otherComp);


    protected: //accessible by derived classes but not external code
        SwappableProcessor* processor = nullptr;

};