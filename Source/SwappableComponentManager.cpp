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

void SwappableComponentManager::initializeComponents() {
    for (int i = 0; i < MAX_COMPONENTS; i++)
    {
        /*
        if (i % 4 == 0)
        {
            swappableComponents.push_back(std::make_unique<GateComponent>(apvts, i));
        }
        else if (i % 4 == 1)
        {
            swappableComponents.push_back(std::make_unique<DistortionComponent>(apvts, i));
        }
        else if (i % 4 == 2)
        {
            swappableComponents.push_back(std::make_unique<FlangerComponent>(apvts, i));
        }
        else {*/
            swappableComponents.push_back(std::make_unique<EmptyComponent>(i));
        //}
        //TODO: some of these calls should be in the swappableComponent constructor
        swappableComponents[i]->addActionListener(this);
        swappableComponents[i]->addActionListener(&audioProcessor);
        swappableComponents[i]->setManager(this);
        //swappableComponents[i]->getProcessor()->assignParamPointers(i);
        swappableComponents[i]->setComponentAttachments(i);
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

            //DBG("index of new component" << index);
            if (index < 0 || index >= swappableComponents.size())
                return;
            if (componentType != "EMPTY" && (componentType == "GATE" || componentType == "DISTORTION" || componentType == "FLANGER")) {

                //delete the EmptyComponent (by changing its pointer, it automatically deletes due to unique_ptr logic), and creates a new one
                if (componentType == "GATE") {
                    swappableComponents[index] = std::make_unique<GateComponent>(apvts,  index);
                }
                else if (componentType == "DISTORTION") {
                    swappableComponents[index] = std::make_unique<DistortionComponent>(apvts, index);
                }
                else if (componentType == "FLANGER") {
                    swappableComponents[index] = std::make_unique<FlangerComponent>(apvts, index);

                }
                //TODO: maybe all of these calls should be in the swappableComponent constructor
                swappableComponents[index]->addActionListener(this);
                swappableComponents[index]->addActionListener(&audioProcessor);
                swappableComponents[index]->setManager(this);
                //swappableComponents[index]->getProcessor()->assignParamPointers(index); //TODO unecessary?  already in constructor
                swappableComponents[index]->setComponentAttachments(index); //TODO: why doesnt this work in the constructor?
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

    const auto draggedIndex = getComponentIndex(draggedComponent);
    const auto otherIndex = getComponentIndex(otherComponent);

    if (!(draggedIndex == -1 || otherIndex == -1))
    {

        

        // swap the param values and swap their pointers
        swapProcessorParams(draggedComponent, otherComponent);

        // Swap components in the component list
        std::swap(components[draggedIndex], components[otherIndex]);
        //swap their processor indexes 
        //draggedComponent.getProcessor()->setProcessorIndex(otherIndex);
        //otherComponent.getProcessor()->setProcessorIndex(draggedIndex);

        //reassign their pointers
        //draggedComponent.getProcessor()->assignParamPointers(otherIndex);
        //otherComponent.getProcessor()->assignParamPointers(draggedIndex);

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
    
    bool bothProcesorsSameType = false;
    //if both components are the same type, simply swap their values, indexes, and pointers
    if (auto* gateProcessorA = dynamic_cast<GateProcessor*>(draggedProcessor)) {
        if (auto* gateProcessorB = dynamic_cast<GateProcessor*>(otherProcessor)) {
            swapGateParams(gateProcessorA, gateProcessorB);
            bothProcesorsSameType = true;
        }
    }
    else if (auto* distortionProcessorA = dynamic_cast<DistortionProcessor*>(draggedProcessor)) {
        if (auto* distortionProcessorB = dynamic_cast<DistortionProcessor*>(otherProcessor)) {
            swapDistortionParams(distortionProcessorA, distortionProcessorB);
            bothProcesorsSameType = true;
        }

    }
    else if (auto* flangerProcessorA = dynamic_cast<FlangerProcessor*>(draggedProcessor)) {
        if (auto* flangerProcessorB = dynamic_cast<FlangerProcessor*>(otherProcessor)) {
            swapFlangerParams(flangerProcessorA, flangerProcessorB);
            bothProcesorsSameType = true;

        }

    }

    if (bothProcesorsSameType) {
        draggedComponent.getProcessor()->setProcessorIndex(otherIndex);
        otherComponent.getProcessor()->setProcessorIndex(draggedIndex);
        draggedComponent.getProcessor()->assignParamPointers(otherIndex);
        otherComponent.getProcessor()->assignParamPointers(draggedIndex);
    }
    else {

        if (auto* distortionProcessor = dynamic_cast<DistortionProcessor*>(draggedProcessor)) {
            moveDistortionParams(distortionProcessor, otherIndex);
        }
        else if (auto* gateProcessor = dynamic_cast<GateProcessor*>(draggedProcessor)) {
            moveGateParams(gateProcessor, otherIndex);
        }
        else if (auto* flangerProcessor = dynamic_cast<FlangerProcessor*>(draggedProcessor)) {
            moveFlangerParams(flangerProcessor, otherIndex);
        }
        
        if (auto* distortionProcessor = dynamic_cast<DistortionProcessor*>(otherProcessor)) {
            moveDistortionParams(distortionProcessor, draggedIndex);
        }
        else if (auto* gateProcessor = dynamic_cast<GateProcessor*>(otherProcessor)) {
            moveGateParams(gateProcessor, draggedIndex);
        }
        else if (auto* flangerProcessor = dynamic_cast<FlangerProcessor*>(otherProcessor)) {
            moveFlangerParams(flangerProcessor, draggedIndex);
        }
    }


}
void SwappableComponentManager::moveDistortionParams(DistortionProcessor* distortionProcessor, int index) {
    //cache current values
    const auto on = distortionProcessor->getOnState();
    const auto amt = distortionProcessor->getAmount();
    const auto type = distortionProcessor->getDistortionType();

    //reset currently pointed to values
    distortionProcessor->setOnState(false);
    distortionProcessor->setAmount(0);
    distortionProcessor->setDistortionType(0);

    //change index and move pointers to new index
    distortionProcessor->setProcessorIndex(index);
    distortionProcessor->assignParamPointers(index);

    //overwrite newly pointed to values
    distortionProcessor->setOnState(on);
    distortionProcessor->setAmount(amt);
    distortionProcessor->setDistortionType(type);

}

void SwappableComponentManager::moveGateParams(GateProcessor* gateProcessor, int index){
    //cache current values
    const auto on = gateProcessor->getOnState();
    const auto thresh = gateProcessor->getThreshold();
    const auto type = gateProcessor->getGateType();
    const auto attack = gateProcessor->getAttack();
    const auto release = gateProcessor->getRelease();
    const auto hold = gateProcessor->getHold();

    //reset currently pointed to values
    gateProcessor->setOnState(false);
    gateProcessor->setThreshold(0.0f);
    gateProcessor->setGateType(0);
    gateProcessor->setAttack(0.0f);
    gateProcessor->setRelease(0.0f);
    gateProcessor->setHold(0.0f);

    //change index and move pointers to new index
    gateProcessor->setProcessorIndex(index);
    gateProcessor->assignParamPointers(index);

    //overwrite newly pointed to values
    gateProcessor->setOnState(on);
    gateProcessor->setThreshold(thresh);
    gateProcessor->setGateType(type);
    gateProcessor->setAttack(attack);
    gateProcessor->setRelease(release);
    gateProcessor->setHold(hold);
}

void SwappableComponentManager::moveFlangerParams(FlangerProcessor* flangerProcessor, int index){
    //cache current values
    const auto on = flangerProcessor->getOnState();
    const auto delay = flangerProcessor->getDelay();
    const auto mix = flangerProcessor->getMix();

    //reset currently pointed to values
    flangerProcessor->setOnState(false);
    flangerProcessor->setDelay(0.0f);
    flangerProcessor->setMix(0.0f);

    //change index and move pointers to new index
    flangerProcessor->setProcessorIndex(index);
    flangerProcessor->assignParamPointers(index);

    //overwrite newly pointed to values
    flangerProcessor->setOnState(on);
    flangerProcessor->setDelay(delay);
    flangerProcessor->setMix(mix);
}

void SwappableComponentManager::swapGateParams(GateProcessor* a, GateProcessor* b)
{
    //cache current values
    const auto a_on = a->getOnState();
    const auto a_thresh = a->getThreshold();
    const auto a_type = a->getGateType();
    const auto a_attack = a->getAttack();
    const auto a_release = a->getRelease();
    const auto a_hold = a->getHold();

    const auto b_on = b->getOnState();
    const auto b_thresh = b->getThreshold();
    const auto b_type = b->getGateType();
    const auto b_attack = b->getAttack();
    const auto b_release = b->getRelease();
    const auto b_hold = b->getHold();

    //swap values
    a->setOnState(b_on);
    a->setThreshold(b_thresh);
    a->setGateType(b_type);
    a->setAttack(b_attack);
    a->setRelease(b_release);
    a->setHold(b_hold);

    b->setOnState(a_on);
    b->setThreshold(a_thresh);
    b->setGateType(a_type);
    b->setAttack(a_attack);
    b->setRelease(a_release);
    b->setHold(a_hold);
}

void SwappableComponentManager::swapDistortionParams(DistortionProcessor* a, DistortionProcessor* b)
{
    //cache current values
    const auto a_on = a->getOnState();
    const auto a_amt = a->getAmount();
    const auto a_type = a->getDistortionType();

    const auto b_on = b->getOnState();
    const auto b_amt = b->getAmount();
    const auto b_type = b->getDistortionType();
    
    //swap values
    a->setOnState(b_on);
    a->setAmount(b_amt);
    a->setDistortionType(b_type);

    b->setOnState(a_on);
    b->setAmount(a_amt);
    b->setDistortionType(a_type);
}

void SwappableComponentManager::swapFlangerParams(FlangerProcessor* a, FlangerProcessor* b)
{
    //cache current values
    const auto a_on = a->getOnState();
    const auto a_delay = a->getDelay();
    const auto a_mix = a->getMix();

    const auto b_on = b->getOnState();
    const auto b_delay = b->getDelay();
    const auto b_mix = b->getMix();

    //swap values
    a->setOnState(b_on);
    a->setDelay(b_delay);
    a->setMix(b_mix);

    b->setOnState(a_on);
    b->setDelay(a_delay);
    b->setMix(a_mix);
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