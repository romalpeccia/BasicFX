/*
  ==============================================================================

    CircularBuffer.h
    Created: 15 Apr 2025 12:17:17am
    Author:  romal

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/*
*/


class CircularBuffer
{
public:
    CircularBuffer::CircularBuffer() {}
    ~CircularBuffer() {};

    void prepare(int numChannels, int maxSamples);
    void write(int channel, float sample);
    float read(int channel, int index) const;
    float readInterpolated(int channel, float index) const;

private:
    std::vector<std::vector<float>> buffers;
    std::vector<int> writeIndices;
    int size = 0;
};