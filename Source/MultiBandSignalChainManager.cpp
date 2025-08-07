/*
  ==============================================================================

    MultiBandSignalChainManager.cpp
    Created: 13 May 2025 10:26:33pm
    Author:  romal

  ==============================================================================
*/

#include "MultiBandSignalChainManager.h"
#include "MultiBandSignalChainProcessor.h"
MultiBandSignalChainManager::MultiBandSignalChainManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts)
    : audioProcessor(p), apvts(_apvts) {
    
    for (int band = 0; band < MAX_BANDS; band++) {
        swappableComponentManagers.push_back(std::make_unique<SwappableComponentManager>(p, apvts, band));
        addAndMakeVisible(swappableComponentManagers.back().get());
    }

    signalChainProcessors = std::make_unique< MultiBandSignalChainProcessor>(this);


}

void MultiBandSignalChainManager::resized() {
    auto bounds = getLocalBounds();
    if (!swappableComponentManagers.empty()) {

        float x = bounds.getX();
        float y = bounds.getY();
        float width = bounds.getWidth() ;
        float height = bounds.getHeight() / swappableComponentManagers.size();
        //start at the top of bounds and iterate, drawing components proportional to the size of the MultiBandSignalChainManager
        for (auto& compPtr : swappableComponentManagers) {
            auto comp = compPtr.get();
            comp->setBounds(x, y, width, height);
            y += height;
        }
    }
}

SwappableComponentManager* MultiBandSignalChainManager::getComponentManager(int index)
{
    if (index >= 0 && index < static_cast<int>(swappableComponentManagers.size()))
        return swappableComponentManagers[index].get();  // return raw pointer

    return nullptr; // index out of range
}
std::vector<std::unique_ptr<SwappableComponentManager>>& MultiBandSignalChainManager::getSwappableComponentManagerList()
{
    return swappableComponentManagers;
}

MultiBandSignalChainProcessor* MultiBandSignalChainManager::getMultiBandSignalChainProcessors()
{
    if (swappableComponentManagers.size() < 1) {
        return nullptr;
    }
    else {
        return signalChainProcessors.get();
    }
}