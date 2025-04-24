/*
  ==============================================================================

    SwappableComponentManager.cpp
    Created: 24 Apr 2025 11:59:21am
    Author:  romal

  ==============================================================================
*/

#include "SwappableComponentManager.h"



SwappableComponentManager::SwappableComponentManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts) : audioProcessor(p), apvts(_apvts) {
    for (int i = 0; i < MAX_COMPONENTS; i++)
    {
        if (i % 4 == 0)
        {
            swappableComponents.push_back(std::make_unique<GateComponent>(apvts, audioProcessor.gateProcessors[i].get(), i));
        }
        else if (i % 4 == 1)
        {
            swappableComponents.push_back(std::make_unique<DistortionComponent>(apvts, audioProcessor.distortionProcessors[i].get(), i));
        }
        else if (i % 4 == 2)
        {
            swappableComponents.push_back(std::make_unique<FlangerComponent>(apvts, audioProcessor.flangerProcessors[i].get(), i));
        }
        else {
            swappableComponents.push_back(std::make_unique<EmptyComponent>(audioProcessor.emptyProcessor.get(), i));
        }
        swappableComponents[i]->addActionListener(this);
        swappableComponents[i]->addActionListener(&p);
        swappableComponents[i]->setManager(this);
        addAndMakeVisible(swappableComponents.back().get());
    }
}


void SwappableComponentManager::actionListenerCallback(const juce::String& message) {
    
    if (message.startsWith("CREATECOMPONENT")) { //called by EmptyComponent.menu.onClick()

        juce::StringArray tokens;
        tokens.addTokens(message, "_", "");
        if (tokens.size() >= 3)
        {
            int index = tokens[1].getIntValue();
            juce::String componentType = tokens[2];


            if (index < 0 || index >= swappableComponents.size())
                return;
            if (componentType != "EMPTY" && (componentType == "GATE" || componentType == "DISTORTION" || componentType == "FLANGER")) {


                //delete the EmptyComponent (by changing its pointer, it automatically deletes due to unique_ptr logic), and create a new one
                if (componentType == "GATE") {
                    swappableComponents[index] = std::make_unique<GateComponent>(apvts, audioProcessor.gateProcessors[index].get(), index);
                }
                else if (componentType == "DISTORTION") {
                    swappableComponents[index] = std::make_unique<DistortionComponent>(apvts, audioProcessor.distortionProcessors[index].get(), index);
                }
                else if (componentType == "FLANGER") {
                    swappableComponents[index] = std::make_unique<FlangerComponent>(apvts, audioProcessor.flangerProcessors[index].get(), index);

                }
                swappableComponents[index]->addActionListener(this);
                swappableComponents[index]->addActionListener(&audioProcessor);
                swappableComponents[index]->setManager(this);
                addAndMakeVisible(swappableComponents[index].get());
                resized();
                audioProcessor.actionListenerCallback(message);//notify the processor that the UI has changed
            }
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
        //start at the corner of bounds and iterate. 
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

    // Find indices of the two components
    const auto indexA = findComponentIndex(draggedComponent);
    const auto indexB = findComponentIndex(otherComponent);

    if (indexA == -1 || indexB == -1)
        return;

    // Swap components in the list
    std::swap(components[indexA], components[indexB]);

    // Swap their positions visually
    const auto boundsA = draggedComponent.getBounds();
    draggedComponent.setBounds(otherComponent.getBounds());
    otherComponent.setBounds(boundsA);

    // Notify processor of the swap
    sendActionMessage("SWAPPED_" + String(indexA) + "_" + String(indexB));
}

int SwappableComponentManager::findComponentIndex(const SwappableComponent& component)
{
    for (int i = 0; i < static_cast<int>(swappableComponents.size()); i++)
        if (swappableComponents[i].get() == &component)
            return i;

    return -1;
}