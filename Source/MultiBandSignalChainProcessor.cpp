/*
  ==============================================================================

    MultiBandSignalChainProcessor.cpp
    Created: 14 May 2025 1:59:01pm
    Author:  romal

  ==============================================================================
*/

#include "MultiBandSignalChainProcessor.h"

MultiBandSignalChainProcessor::MultiBandSignalChainProcessor(MultiBandSignalChainManager* signalChainComponents) {
    signalChains.resize(MAX_BANDS); //NOTE: did this fix it?
    signalChains.clear();
    for (int i = 0; i < MAX_BANDS; i++) {
        signalChains.push_back(std::make_unique<SignalChainProcessor>(signalChainComponents->getComponentManager(i)));

    }
}

