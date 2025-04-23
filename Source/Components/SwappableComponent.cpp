/*
  ==============================================================================

    SwappableComponent.cpp
    Created: 15 Apr 2025 8:30:22pm
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponent.h"
#include "../PluginEditor.h"


SwappableComponent::~SwappableComponent() {
    swappableComponentList.erase(
        std::remove(swappableComponentList.begin(), swappableComponentList.end(), this),
        swappableComponentList.end()
    );
}

void SwappableComponent::mouseDown(const juce::MouseEvent& e)
{
    oldBounds = getBounds();
    componentDragger.startDraggingComponent(this, e);
}

void SwappableComponent::mouseDrag(const juce::MouseEvent& e)
{
    componentDragger.dragComponent(this, e, nullptr);
    newBounds = getBounds();
}

void SwappableComponent::mouseUp(const juce::MouseEvent& e)  {

    SwappableComponent* componentToSwap = nullptr;
    int largestIntersectionArea = 0;
    for (auto* comp : swappableComponentList) {
        if (comp != nullptr && comp != this) {
            auto intersection = newBounds.getIntersection(comp->getBounds());

            int area = intersection.getWidth() * intersection.getHeight();
            if (area > largestIntersectionArea && area > area_overlap_threshold) {
                largestIntersectionArea = area;
                componentToSwap = comp;
            }
        }
    }
    if (componentToSwap != nullptr) {
        setBounds(oldBounds);
        swapComponents(componentToSwap);
        sendChangeMessage();
    }
    else {
        setBounds(oldBounds);
    }
}

void SwappableComponent::swapComponents(SwappableComponent* otherComp)
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

void SwappableComponent::setBounds(juce::Rectangle<int> bounds)
{
    juce::Component::setBounds(bounds);
    setAreaOverLapThreshold();
}
void SwappableComponent::setBounds(int x, int y, int width, int height)
{
    juce::Component::setBounds(x, y, width, height);
    setAreaOverLapThreshold();
}
void SwappableComponent::setAreaOverLapThreshold() {
    area_overlap_threshold = (getLocalBounds().getWidth() * getLocalBounds().getHeight()) / 2;
}