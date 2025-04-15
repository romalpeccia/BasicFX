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
    juce::ComponentDragger myDragger;
    inline static std::vector<SwappableComponent*> swappableComponentList; // shared vector across all instances of swappable Components
    juce::Rectangle<int> oldBounds;
    juce::Rectangle<int> newBounds;
    const int areaOverLapThreshold = 50;

    SwappableComponent() {
        swappableComponentList.push_back(this);
    }

    ~SwappableComponent() override {
        swappableComponentList.erase(
            std::remove(swappableComponentList.begin(), swappableComponentList.end(), this),
            swappableComponentList.end()
        ); //TODO: is this really the best way to do this? remove reorders the list to put the component at the end, and erase deletes that component
    }

    void swapComponents(SwappableComponent* otherComp)
    {
        //swap the bounds
        auto otherCompBounds = getBounds();
        setBounds(otherComp->getBounds());
        otherComp->setBounds(otherCompBounds);

        //swap the components in the list
        auto& list = swappableComponentList;
        auto itA = std::find(list.begin(), list.end(), this);
        auto itB = std::find(list.begin(), list.end(), otherComp);

        if (itA != list.end() && itB != list.end())
            std::iter_swap(itA, itB);
    }

    void mouseDown(const juce::MouseEvent& e)
    {
        oldBounds = getBounds();
        myDragger.startDraggingComponent(this, e);
    }

    void mouseDrag(const juce::MouseEvent& e)
    {

        myDragger.dragComponent(this, e, nullptr);
        newBounds = getBounds();
    }
    void mouseUp(const juce::MouseEvent& e) {

        SwappableComponent* componentToSwap = nullptr;
        int largestIntersectionArea = 0;
        for (auto* comp : swappableComponentList) {
            if (comp != nullptr && comp != this) {
                auto intersection = newBounds.getIntersection(comp->getBounds());

                int area = intersection.getWidth() * intersection.getHeight();
                if (area > largestIntersectionArea && area > areaOverLapThreshold) {
                    largestIntersectionArea = area;
                    componentToSwap = comp;
                }
            }
        }
        if (componentToSwap != nullptr) {
            setBounds(oldBounds);
            swapComponents(componentToSwap);
        }
        else {
            setBounds(oldBounds);
        }
    }


};