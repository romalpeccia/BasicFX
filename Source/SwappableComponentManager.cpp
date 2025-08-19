/*
  ==============================================================================

    SwappableComponentManager.cpp
    Created: 24 Apr 2025 11:59:21am
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponentManager.h"



SwappableComponentManager::SwappableComponentManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts) : audioProcessor(p), apvts(_apvts) {
   
    initializeComponents();
}

SwappableComponentManager::SwappableComponentManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts, int _bandIndex) 
    : audioProcessor(p), apvts(_apvts), bandIndex(_bandIndex) {
    initializeComponents();
}

void SwappableComponentManager::initializeComponents() {
    bool paramsFromMemory = false;
    if (bandIndex >= 0) {
        if (!paramsFromMemory) {
            //create a bunch of empty components
            for (int i = 0; i < MAX_COMPONENTS; i++)
            {
                swappableComponents.push_back(std::make_unique<EmptyComponent>(i, this));
                //TODO: some of these calls should be in the swappableComponent constructor
                swappableComponents[i]->addActionListener(this);
                swappableComponents[i]->addActionListener(&audioProcessor);
                //swappableComponents[i]->getProcessor()->assignParamPointers(i);
                swappableComponents[i]->setComponentAttachments(i);
                addAndMakeVisible(swappableComponents.back().get());
            }
        }

    }
    else {
        /*
        if (!paramsFromMemory) {
            //create a bunch of empty components
            for (int i = 0; i < MAX_COMPONENTS; i++)
            {
                swappableComponents.push_back(std::make_unique<EmptyComponent>(i));
                //TODO: some of these calls should be in the swappableComponent constructor
                swappableComponents[i]->addActionListener(this);
                swappableComponents[i]->addActionListener(&audioProcessor);
                //swappableComponents[i]->getProcessor()->assignParamPointers(i);
                swappableComponents[i]->setComponentAttachments(i);
                addAndMakeVisible(swappableComponents.back().get());
            }
        }
        else {
            //TODO: implement this when memory save/loading is implemented
        }
        */
    }
}




void SwappableComponentManager::actionListenerCallback(const juce::String& message) {
    
    if (message.startsWith("CREATECOMPONENT")) { //called by EmptyComponent.menu.onChange()

        juce::StringArray tokens;
        tokens.addTokens(message, "_", "");
        if (tokens.size() == 3)
        {
            int index = tokens[1].getIntValue();
            juce::String componentType = tokens[2];

            if (index < 0 || index > MAX_COMPONENTS || index >= swappableComponents.size())
                return;
            if (componentType != "-" && (componentType == "GATE" || componentType == "DISTORTION" || componentType == "FLANGER" || componentType == "EQ" || componentType == "UTILITY")) {

                //replace the EmptyComponent (by changing its pointer, it automatically deletes due to unique_ptr logic) with the new Component
                if (componentType == "GATE") {
                    swappableComponents[index] = std::make_unique<GateComponent>(apvts,  index, this);
                }
                else if (componentType == "DISTORTION") {
                    swappableComponents[index] = std::make_unique<DistortionComponent>(apvts, index, this);
                }
                else if (componentType == "FLANGER") {
                    swappableComponents[index] = std::make_unique<FlangerComponent>(apvts, index, this);
                }
                else if (componentType == "EQ") {
                    swappableComponents[index] = std::make_unique<EQComponent>(apvts, index, this);
                }
                else if (componentType == "UTILITY") {
                    swappableComponents[index] = std::make_unique<UtilityComponent>(apvts, index, this);
                }
                //TODO: maybe some of these calls should be in the swappableComponent constructor
                swappableComponents[index]->addActionListener(this);
                swappableComponents[index]->addActionListener(&audioProcessor);
                //swappableComponents[index]->getProcessor()->assignParamPointers(index); //TODO unecessary?  already in constructor
                swappableComponents[index]->setComponentAttachments(index); //TODO: why doesnt this work in the constructor?
                addAndMakeVisible(swappableComponents[index].get());
                resized();
                audioProcessor.actionListenerCallback(message);//notify the processor that the UI has changed
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

            swappableComponents[index] = std::make_unique<EmptyComponent>(index, this);
            swappableComponents[index]->addActionListener(this);
            swappableComponents[index]->addActionListener(&audioProcessor);
            addAndMakeVisible(swappableComponents[index].get());
            resized();
            audioProcessor.actionListenerCallback(message);//notify the processor that the UI has changed
            }
        }
}

void SwappableComponentManager::resized() {
    auto bounds = getLocalBounds();
    if (!swappableComponents.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() / swappableComponents.size();
        float height = bounds.getHeight();
        //start at the corner of bounds and iterate, drawing components proportional to the size of the SwappableComponentManager
        for (auto& compPtr : swappableComponents) {
            auto comp = compPtr.get();
            comp->setBounds(x, y, width, height);
            x += width;
        }
    }
}

void SwappableComponentManager::handleDraggedComponent(SwappableComponent& draggedComp) {

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

void SwappableComponentManager::swapComponents(SwappableComponent& draggedComponent, SwappableComponent& otherComponent)
{
    auto& components = swappableComponents;

    const auto draggedIndex = getComponentIndex(draggedComponent);
    const auto otherIndex = getComponentIndex(otherComponent);

    if (!(draggedIndex == -1 || otherIndex == -1))
    {
        // swap the param values and swap their pointers
        swapProcessorParams(draggedComponent, otherComponent);

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

void SwappableComponentManager::swapProcessorParams(SwappableComponent& draggedComponent, SwappableComponent& otherComponent)
{
    SwappableProcessor* draggedProcessor = draggedComponent.getProcessor();
    SwappableProcessor* otherProcessor = otherComponent.getProcessor();
    int draggedIndex = getComponentIndex(draggedComponent);
    int otherIndex = getComponentIndex(otherComponent);

    if (typeid(*draggedProcessor) == typeid(*otherProcessor)) {
        // if both components are the same type, simply swap their values, indexes, and pointers

        draggedProcessor->swapParamValues(otherProcessor);

        draggedProcessor->setProcessorIndex(otherIndex);
        otherProcessor->setProcessorIndex(draggedIndex);
        draggedProcessor->assignParamPointers(otherIndex);
        otherProcessor->assignParamPointers(draggedIndex);
    }
    else {
        // otherwise, reset the current values and then swap the indexes and pointers (internally), and values
        draggedProcessor->moveParamValues(otherIndex);
        otherProcessor->moveParamValues(draggedIndex);
    }
}


std::vector<SwappableComponent*> SwappableComponentManager::getComponentList()
{
    //returns all components in std::vector<std::unique_ptr<SwappableComponent>> swappableComponents
    std::vector<SwappableComponent*> list;
    for (auto& compPtr : swappableComponents)
        list.push_back(compPtr.get());
    return list;
}

int SwappableComponentManager::getComponentIndex(const SwappableComponent& component)
{
    //returns index of a component if it is in std::vector<std::unique_ptr<SwappableComponent>> swappableComponents
    for (int i = 0; i < swappableComponents.size(); i++)
        if (swappableComponents[i].get() == &component)
            return i;

    return -1;
}