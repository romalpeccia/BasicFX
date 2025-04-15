/*
  ==============================================================================

    VisualizerProcessor.h
    Created: 15 Apr 2025 1:52:15pm
    Author:  romal

  ==============================================================================
*/

#pragma once
#include "../CircularBuffer.h"
#include "../Utilities.h"


class VisualizerProcessor {
    public:

        VisualizerProcessor(int _maxSamples) : maxSamples(_maxSamples) {
            sampleBuffer.prepare(1, maxSamples);
        }
        ~VisualizerProcessor() {};

        void prepareToPlay(int sampleRate, int _totalNumInputChannels) {
            sampleBuffer.prepare(1, maxSamples);
        };
        void processBlock(juce::AudioBuffer<float>& buffer) {

            int numChannels = buffer.getNumChannels();
            int numSamples = buffer.getNumSamples();
            
            for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
                if (sampleNum % 75 == 0) {
                    float sample = 0.f;
                    for (int channel = 0; channel < numChannels; ++channel) {
                        sample += buffer.getSample(channel, sampleNum); //TODO: this seems easier than making a channelData pointer every time, maybe clean up my other stuff?
                    }
                    sample /= numChannels; //TODO: average across channels? better way to do this?

                    sampleBuffer.write(0, sample);
                }
            }
        }

        float getCurrentValue() { return currentValue; }
        void setMaxSamples(int _maxSamples) { 
            maxSamples = _maxSamples; 
            sampleBuffer.prepare(1, maxSamples);
        }

        CircularBuffer sampleBuffer;//TODO: make this private and return its member variables
    private:
        float currentValue = 0;
        float sampleRate = 44100.0f;
        int totalNumInputChannels = 1;
        int maxSamples; 

};
