/*
  ==============================================================================

    SwappableComponent.cpp
    Created: 15 Apr 2025 8:30:22pm
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponent.h"
#include "../PluginEditor.h"
#include "../MultiBandSignalChainProcessor.h"
SwappableComponent::SwappableComponent(std::unique_ptr<SwappableProcessor> processorPtr) : processor(std::move(processorPtr)) {

    xButton.onClick = [this]() {
        int index = swappableComponentManager->getComponentIndex(*this);
        //signal the ComponentManager to do something
        sendActionMessage("DELETECOMPONENT_" + String(index));
        };

    addAndMakeVisible(xButton);
}
SwappableComponent::~SwappableComponent() {

}

void SwappableComponent::resized(){
    auto bounds = getLocalBounds();
    xButton.setBounds(bounds.withTrimmedBottom(bounds.getHeight() * 0.95).withTrimmedLeft(bounds.getWidth() * 0.75));
}


void SwappableComponent::paint(juce::Graphics & g){

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