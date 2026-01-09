/*
  ==============================================================================

    SignalChainProcessor.cpp
    Created: 9 Oct 2025 1:45:47pm
    Author:  romal

  ==============================================================================
*/

#include "SignalChainProcessor.h"
void SignalChainProcessor::actionListenerCallback(const juce::String& message) {
    juce::StringArray tokens;
    tokens.addTokens(message, "_", "");
    if (message.startsWith("CREATECOMPONENT")) {
        //create effect processor

        if (tokens.size() == 3)
        {
            int index = tokens[1].getIntValue();
            juce::String componentType = tokens[2];
            if ((index >= 0 && index < MAX_COMPONENTS) && (componentType == "EMPTY" || 
                componentType == "GATE" || componentType == "DISTORTION" || componentType == "FLANGER" || componentType == "EQ")) {
                if (componentType == "GATE") {
                    signalChain[index] = std::move(std::make_unique<GateProcessor>(apvts, index));
                }
                else if (componentType == "DISTORTION") {
                    signalChain[index] = std::move(std::make_unique<DistortionProcessor>(apvts, index));
                }
                else if (componentType == "FLANGER") {
                    signalChain[index] = std::move(std::make_unique<FlangerProcessor>(apvts, index));
                }
                else if (componentType == "EQ") {
                    signalChain[index] = std::move(std::make_unique<EQProcessor>(apvts, index));
                }
                else if (componentType == "EMPTY") {
                    signalChain[index] = std::move(std::make_unique<EmptyProcessor>());
                }
            }
        }
    }
    else if (message.startsWith("DELETECOMPONENT")) {
        //create empty processor
        if (tokens.size() == 2)
        {
            int index = tokens[1].getIntValue();
            if (index >=0 && index < MAX_COMPONENTS){
                signalChain[index] = std::move(std::make_unique<EmptyProcessor>());
            }
        }
    }
    else if (message.startsWith("SWAPPED")) {
        if (tokens.size() == 3) {
            int draggedIndex = tokens[1].getIntValue();
            int otherIndex = tokens[2].getIntValue();
            if (draggedIndex >= 0 && draggedIndex < MAX_COMPONENTS && otherIndex >= 0 && otherIndex < MAX_COMPONENTS) {
                swapProcessorParams(draggedIndex, otherIndex);
            }
        }
    }
    else if (message.startsWith("UPDATE")) {
        if (tokens.size() == 3) {
            int index = tokens[1].getIntValue();
            if (index >= 0 && index < MAX_COMPONENTS) {
                signalChain[index]->updateFilters();
            }
        }
    }

}

void SignalChainProcessor::swapProcessorParams(int draggedIndex, int otherIndex) {
    DBG(draggedIndex);
    DBG(otherIndex);
    SwappableProcessor* draggedProcessor = signalChain[draggedIndex].get();
    SwappableProcessor* otherProcessor = signalChain[otherIndex].get();

    if (draggedIndex >= 0 && otherIndex >= 0 && draggedIndex < signalChain.size() && otherIndex < signalChain.size()) {

        if (typeid(*draggedProcessor) == typeid(*otherProcessor)) {
            // if both components are the same type, simply swap their values and pointers
            draggedProcessor->swapParamValues(otherProcessor);

            //draggedProcessor->setProcessorIndex(otherIndex); 
            //otherProcessor->setProcessorIndex(draggedIndex);
            draggedProcessor->assignParamPointers(otherIndex); 
            otherProcessor->assignParamPointers(draggedIndex);
        }
        else {
            // otherwise, reset the current values and then swap the pointers and values
            draggedProcessor->moveParamValues(otherIndex);
            otherProcessor->moveParamValues(draggedIndex);
        }
        std::swap(signalChain[draggedIndex], signalChain[otherIndex]); //swap the processors in the vector
    }
}