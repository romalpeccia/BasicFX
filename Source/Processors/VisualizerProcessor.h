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
        VisualizerProcessor() {};
        VisualizerProcessor(int _maxSamples) : maxSamples(_maxSamples) {
        }
        ~VisualizerProcessor() {};


        void processBlock(juce::AudioBuffer<float>& buffer) {

            int numChannels = buffer.getNumChannels();
            int numSamples = buffer.getNumSamples();

            //const float* channelData = buffer.getReadPointer(1);
            //currentValue = channelData[0]; //just take the first value for simplicity
            
            //currentValue = calculateAverageAcrossChannels(buffer);

            //take peak negative or positive value in buffer
            /*
            currentValue = 0.0f;
            for (int channel = 0; channel < buffer.getNumChannels(); channel++) {
                const float* channelData = buffer.getReadPointer(channel);
                for (int sampleNum = 0; sampleNum < numSamples; sampleNum++) {
                    if (fabs(channelData[channel]) > fabs(currentValue)) {
                        currentValue = channelData[channel];
                    }
                }
            }*/
            
            for (int sampleNum = 0; sampleNum < numSamples; ++sampleNum) {
                float sample = 0.0f;
                for (int channel = 0; channel < numChannels; ++channel)
                    sample += buffer.getSample(channel, sampleNum); //TODO: this seems easier than making a channelData pointer every time, maybe clean up my other stuff?
                sample /= numChannels; // average across channels

                sampleBuffer.write(0, sample); 
            }
        }
        float getCurrentValue() { return currentValue; }
        void setMaxSamples(int _maxSamples) { 
            maxSamples = _maxSamples; 
            sampleBuffer.prepare(totalNumInputChannels, maxSamples * 5); 
        }

        CircularBuffer sampleBuffer;//TODO: make this private and return its member variables
    private:
        float currentValue = 0;
        float sampleRate = 44100.0f;
        int totalNumInputChannels = 1;
        int maxSamples = 10000000000; //TODO: this is a janky fix for the buffer passing in a maxSamples value when first created by the visualizerComponent. maybe make a preparetoplay function?

};
