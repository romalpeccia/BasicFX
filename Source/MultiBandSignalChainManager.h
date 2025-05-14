/*
  ==============================================================================

    MultiBandSignalChainManager.h
    Created: 13 May 2025 10:26:33pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
//#include "MultiBandSignalChainProcessor.h"
#include "PluginProcessor.h"
#include "SwappableComponentManager.h"
class MultiBandSignalChainProcessor;

class MultiBandSignalChainManager : public juce::Component {
public:
    MultiBandSignalChainManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts);
        
    ~MultiBandSignalChainManager() {};
    
   

    void resized() override;
    SwappableComponentManager* getComponentManager(int index);

    std::vector<std::unique_ptr<SwappableComponentManager>>& getComponentList();
    MultiBandSignalChainProcessor* getMultiBandSignalChainProcessors();

private:
    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;

    std::unique_ptr< MultiBandSignalChainProcessor> signalChainProcessors = nullptr; 
    //todo renamethis to multibandSignalChainProcessors (also figure out if it belongs here or in editor)
    std::vector <std::unique_ptr<SwappableComponentManager>> signalChainComponents;
};