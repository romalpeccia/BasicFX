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

}

void SwappableComponent::mouseDown(const juce::MouseEvent& e)
{   //called when the component is clicked on
    initialBounds = getBounds();
    componentDragger.startDraggingComponent(this, e);
}

void SwappableComponent::mouseDrag(const juce::MouseEvent& e)
{   //called while the component is being dragged
    componentDragger.dragComponent(this, e, nullptr);
    draggedBounds = getBounds();
}

void SwappableComponent::mouseUp(const juce::MouseEvent& e)  {
    //called while the component has stopped being dragged

    setBounds(initialBounds);
    if (swappableComponentManager != nullptr) {
        swappableComponentManager->handleDraggedComponent(*this);
    }

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
    areaOverlapThreshold = (getLocalBounds().getWidth() * getLocalBounds().getHeight()) / 2;
}


void SwappableComponent::setManager(SwappableComponentManager* _swappableComponentManager) {
    swappableComponentManager = _swappableComponentManager;
}

SwappableComponentManager* SwappableComponent::getManager() const {
    return swappableComponentManager;
}