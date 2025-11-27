/*
  ==============================================================================

    SwappableComponent.cpp
    Created: 15 Apr 2025 8:30:22pm
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponent.h"
#include "../PluginEditor.h"

SwappableComponent::SwappableComponent() {

    xButton.onClick = [this]() {
        int index = signalChainComponent->getComponentIndex(*this);
        //signal the ComponentManager to replace this component with an EmptyComponent
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
    if (signalChainComponent != nullptr) {
        signalChainComponent->handleDraggedComponent(*this);
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


void SwappableComponent::setSignalChainComponent(SignalChainComponent* _signalChainComponent) {
    signalChainComponent = _signalChainComponent;
}

SignalChainComponent* SwappableComponent::getSignalChainComponent() const {
    return signalChainComponent;
}

int SwappableComponent::getIndex() {
    if (signalChainComponent != nullptr) {
        return signalChainComponent->getComponentIndex(*this);
    }
    else {
        return -1;
    }
}