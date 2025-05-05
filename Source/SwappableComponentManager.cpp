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
        // Swap components in the component list
        std::swap(components[draggedIndex], components[otherIndex]);
        

        // swap the values from the old pointers
        swapProcessorParamsIfSameType(draggedComponent.getProcessor(), otherComponent.getProcessor(), otherIndex, draggedIndex);

        //swap their processor indexes (unecessary unless I start using it for something)
        draggedComponent.getProcessor()->setProcessorIndex(otherIndex);
        otherComponent.getProcessor()->setProcessorIndex(draggedIndex);

        //reassign their pointers
        draggedComponent.getProcessor()->assignParamPointers(otherIndex);
        otherComponent.getProcessor()->assignParamPointers(draggedIndex);

        //swap the attachments of components to apvts Params
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



void SwappableComponentManager::swapProcessorParamsIfSameType(SwappableProcessor* a, SwappableProcessor* b, int otherIndex, int draggedIndex)
{
    if (auto* gateA = dynamic_cast<GateProcessor*>(a)) {
        if (auto* gateB = dynamic_cast<GateProcessor*>(b)) {

            float a_on = gateA->getOnState();
            float a_thresh = gateA->getThreshold();
            int a_type = gateA->getGateType();
            float a_attack = gateA->getAttack();
            float a_release = gateA->getRelease();
            float a_hold = gateA->getHold();

            float b_on = gateB->getOnState();
            float b_thresh = gateB->getThreshold();
            int b_type = gateB->getGateType();
            float b_attack = gateB->getAttack();
            float b_release = gateB->getRelease();
            float b_hold = gateB->getHold();

            gateA->setOnState(b_on);
            gateA->setThreshold(b_thresh);
            gateA->setGateType(b_type);
            gateA->setAttack(b_attack);
            gateA->setRelease(b_release);
            gateA->setHold(b_hold);

            gateB->setOnState(a_on);
            gateB->setThreshold(a_thresh);
            gateB->setGateType(a_type);
            gateB->setAttack(a_attack);
            gateB->setRelease(a_release);
            gateB->setHold(a_hold);
            return;
        }
    }

    if (auto* distA = dynamic_cast<DistortionProcessor*>(a)) {
        if (auto* distB = dynamic_cast<DistortionProcessor*>(b)) {
            DBG("==== APVTS STATE before FIX ====");
            DBG(apvts.state.toXmlString());
            float a_on = distA->getOnState();
            float a_amt = distA->getAmount();
            int a_type = distA->getDistortionType();

            float b_on = distB->getOnState();
            float b_amt = distB->getAmount();
            int b_type = distB->getDistortionType();


            distA->setOnState(b_on);
            distA->setAmount(b_amt);
            distA->setDistortionType(b_type);

            distB->setOnState(a_on);
            distB->setAmount(a_amt);
            distB->setDistortionType(a_type);
            DBG("==== APVTS STATE AFTER FIX ====");
            DBG(apvts.state.toXmlString());
            return;
        }
    }

    if (auto* flangerA = dynamic_cast<FlangerProcessor*>(a)) {
        if (auto* flangerB = dynamic_cast<FlangerProcessor*>(b)) {
            float a_delay = flangerA->getDelay();
            float a_mix = flangerA->getMix();
            bool a_on = flangerA->getOnState();

            float b_delay = flangerB->getDelay();
            float b_mix = flangerB->getMix();
            bool b_on = flangerB->getOnState();

            flangerA->setDelay(b_delay);
            flangerA->setMix(b_mix);
            flangerA->setOnState(b_on);

            flangerB->setDelay(a_delay);
            flangerB->setMix(a_mix);
            flangerB->setOnState(a_on);
            return;
        }
    }

    DBG("swapProcessorParamsIfSameType(): processor types don't match or unsupported.");
}

std::vector<SwappableComponent*> SwappableComponentManager::getComponentList()
{
    std::vector<SwappableComponent*> list;
    for (auto& compPtr : swappableComponents)
        list.push_back(compPtr.get());
    return list;
}

int SwappableComponentManager::getComponentIndex(const SwappableComponent& component)
{
    for (int i = 0; i < swappableComponents.size(); i++)
        if (swappableComponents[i].get() == &component)
            return i;

    return -1;
}