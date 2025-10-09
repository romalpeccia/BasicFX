/*
  ==============================================================================

    SignalChainComponent.cpp
    Created: 24 Apr 2025 11:59:21am
    Author:  romal

  ==============================================================================
*/

#include "SignalChainComponent.h"



SignalChainComponent::SignalChainComponent(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts) : audioProcessor(p), apvts(_apvts) {
    addActionListener(p.signalChainProcessor.get());
    initializeComponents();
}

void SignalChainComponent::initializeComponents() {
    bool paramsFromMemory = false;

    if (!paramsFromMemory) {
        //create a bunch of empty components
        for (int i = 0; i < MAX_COMPONENTS; i++)
        {
            swappableComponents.push_back(std::make_unique<EmptyComponent>(i));
            swappableComponents[i]->addActionListener(this);
            swappableComponents[i]->addActionListener(audioProcessor.signalChainProcessor.get());
            swappableComponents[i]->setSignalChainComponent(this);
            swappableComponents[i]->setComponentAttachments(i);
            addAndMakeVisible(swappableComponents.back().get());

            juce::String message = "CREATECOMPONENT_" + String(i) + "_EMPTY";
            audioProcessor.signalChainProcessor->actionListenerCallback(message);
        }
    }
    else {
        //TODO: implement this when memory save/loading is implemented
    }

}

void SignalChainComponent::actionListenerCallback(const juce::String& message) {

    if (message.startsWith("CREATECOMPONENT")) { //called by EmptyComponent.menu.onChange()

        juce::StringArray tokens;
        tokens.addTokens(message, "_", "");
        if (tokens.size() == 3)
        {
            int index = tokens[1].getIntValue();
            juce::String componentType = tokens[2];

            if (index < 0 || index > MAX_COMPONENTS || index >= swappableComponents.size())
                return;
            if (componentType != "EMPTY" && (componentType == "GATE" || componentType == "DISTORTION" || componentType == "FLANGER" || componentType == "EQ")) {

                //replace the EmptyComponent (by changing its pointer, it automatically deletes due to unique_ptr logic) with the new Component
                if (componentType == "GATE") {
                    swappableComponents[index] = std::make_unique<GateComponent>(apvts, index);
                }
                else if (componentType == "DISTORTION") {
                    swappableComponents[index] = std::make_unique<DistortionComponent>(apvts, index);
                }
                else if (componentType == "FLANGER") {
                    swappableComponents[index] = std::make_unique<FlangerComponent>(apvts, index);
                }
                else if (componentType == "EQ") {
                    swappableComponents[index] = std::make_unique<EQComponent>(apvts, index);
                }
                //TODO: maybe some of these calls should be in the swappableComponent constructor ?
                //TODO make this a function
                swappableComponents[index]->addActionListener(this); //for SwappableComponent.xButton to signal a delete
                swappableComponents[index]->addActionListener(audioProcessor.signalChainProcessor.get());
                swappableComponents[index]->setSignalChainComponent(this); 
                swappableComponents[index]->setComponentAttachments(index); //TODO: why doesnt this work in the constructor?

                addAndMakeVisible(swappableComponents[index].get());
                resized();
                audioProcessor.signalChainProcessor->actionListenerCallback(message);//notify the processor that the UI has changed
            }
        }
    }
    if (message.startsWith("DELETECOMPONENT")) { //called by SwappableComponent.xButton.onClick()

        juce::StringArray tokens;
        tokens.addTokens(message, "_", "");
        if (tokens.size() == 2)
        {
            int index = tokens[1].getIntValue();
            if (index < 0 || index > MAX_COMPONENTS || index >= swappableComponents.size())
                return;

            swappableComponents[index] = std::make_unique<EmptyComponent>(index);
            swappableComponents[index]->addActionListener(this);
            swappableComponents[index]->addActionListener(audioProcessor.signalChainProcessor.get());
            swappableComponents[index]->setSignalChainComponent(this);
            addAndMakeVisible(swappableComponents[index].get());
            resized();
            audioProcessor.signalChainProcessor->actionListenerCallback(message); //notify the processor that the UI has changed
        }
    }
}

void SignalChainComponent::resized() {
    auto bounds = getLocalBounds();
    if (!swappableComponents.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() / swappableComponents.size();
        float height = bounds.getHeight();
        //start at the corner of bounds and iterate, drawing components proportional to the size of the SignalChainComponent
        for (auto& compPtr : swappableComponents) {
            auto comp = compPtr.get();
            comp->setBounds(x, y, width, height);
            x += width;
        }
    }
}

void SignalChainComponent::handleDraggedComponent(SwappableComponent& draggedComp) {

    SwappableComponent* componentToSwap = nullptr;
    int largestIntersectionArea = 0;
    for (auto& compPtr : swappableComponents) {
        auto comp = compPtr.get();
        if (comp != nullptr && comp != &draggedComp) {
            //compare overlap in area of other components with this component
            auto intersection = draggedComp.getDraggedBounds().getIntersection(comp->getBounds());
            int area = intersection.getWidth() * intersection.getHeight();

            if (area > largestIntersectionArea && area > draggedComp.getAreaOverLapThreshold()) {
                largestIntersectionArea = area;
                componentToSwap = comp;
            }
        }
    }
    if (componentToSwap != nullptr) {
        swapComponents(draggedComp, *componentToSwap);
    }
}

void SignalChainComponent::swapComponents(SwappableComponent& draggedComponent, SwappableComponent& otherComponent)
{
    auto& components = swappableComponents;

    const auto draggedIndex = getComponentIndex(draggedComponent);
    const auto otherIndex = getComponentIndex(otherComponent);

    if (!(draggedIndex == -1 || otherIndex == -1))
    {
        // Swap components in the component list
        std::swap(components[draggedIndex], components[otherIndex]);

        //reset their attachments
        otherComponent.setComponentAttachments(draggedIndex);
        draggedComponent.setComponentAttachments(otherIndex);

        // Swap their positions in UI
        const auto draggedBounds = draggedComponent.getBounds();
        draggedComponent.setBounds(otherComponent.getBounds());
        otherComponent.setBounds(draggedBounds);

        // Notify processor of the swap
        sendActionMessage("SWAPPED_" + String(draggedIndex) + "_" + String(otherIndex));
    }
}

std::vector<SwappableComponent*> SignalChainComponent::getComponentList()
{
    //returns all components in std::vector<std::unique_ptr<SwappableComponent>> swappableComponents
    std::vector<SwappableComponent*> list;
    for (auto& compPtr : swappableComponents)
        list.push_back(compPtr.get());
    return list;
}

int SignalChainComponent::getComponentIndex(const SwappableComponent& component)
{
    //returns index of a component if it is in std::vector<std::unique_ptr<SwappableComponent>> swappableComponents
    for (int i = 0; i < swappableComponents.size(); i++)
        if (swappableComponents[i].get() == &component)
            return i;

    return -1;
}