/*
  ==============================================================================

    SwappableProcessor.cpp
    Created: 16 Apr 2025 12:55:18am
    Author:  romal

  ==============================================================================
*/

#include "SwappableProcessor.h"
#include "../SignalChainProcessor.h" 


void SwappableProcessor::setSignalChainProcessor(SignalChainProcessor* _signalChainProcessor) {
    signalChainProcessor = _signalChainProcessor;
}

SignalChainProcessor* SwappableProcessor::getSignalChainProcessor() const {
    return signalChainProcessor;
}

int SwappableProcessor::getIndex() {
    if (signalChainProcessor != nullptr) {
        return signalChainProcessor->getProcessorIndex(*this);
    }
    else {
        return -1;
    }
}