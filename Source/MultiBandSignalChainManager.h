/*
  ==============================================================================

    MultiBandSignalChainManager.h
    Created: 13 May 2025 10:26:33pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "SwappableComponentManager.h"

const int NUM_BANDS = 4;
class MultiBandSignalChainManager : public juce::Component {
public:
    MultiBandSignalChainManager(BasicFXAudioProcessor& p, juce::AudioProcessorValueTreeState& _apvts);
    ~MultiBandSignalChainManager() {}
    ;
    std::vector <std::unique_ptr<SwappableComponentManager>> signalChainComponents;

    void resized() override;

private:
    BasicFXAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& apvts;
};