/*
  ==============================================================================

    MultiBandSignalChainManager.cpp
    Created: 13 May 2025 10:26:33pm
    Author:  romal

  ==============================================================================
*/

#include "MultiBandSignalChainManager.h"

MultiBandSignalChainManager::MultiBandSignalChainManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts)
    : audioProcessor(p), apvts(_apvts) {
    
    for (int band = 0; band < NUM_BANDS; band++) {
        signalChainComponents.push_back(std::make_unique<SwappableComponentManager>(p, apvts));
        addAndMakeVisible(signalChainComponents.back().get());
    }

}

void MultiBandSignalChainManager::resized() {
    auto bounds = getLocalBounds();
    if (!signalChainComponents.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() ;
        float height = bounds.getHeight() / signalChainComponents.size();
        //start at the top of bounds and iterate, drawing components proportional to the size of the MultiBandSignalChainManager
        for (auto& compPtr : signalChainComponents) {
            auto comp = compPtr.get();
            comp->setBounds(x, y, width, height);
            y += height;
        }
    }
}