/*
  ==============================================================================

    SwappableComponent.h
    Created: 14 Apr 2025 7:56:50pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class SwappableComponent : public juce::Component {
    public:
        SwappableComponent();
        ~SwappableComponent();
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;
        void swapComponents(SwappableComponent* otherComp);
    private:
        juce::ComponentDragger componentDragger; //adding this member allows us to drag the component using the mouseEvent that triggered it
        inline static std::vector<SwappableComponent*> swappableComponentList; // shared vector across all instances of swappable Components
        juce::Rectangle<int> oldBounds;
        juce::Rectangle<int> newBounds;
        const int AREA_OVERLAP_THRESHOLD = 100;
};