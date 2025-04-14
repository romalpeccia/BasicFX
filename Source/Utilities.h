/*
  ==============================================================================

    Utilities.h
    Created: 7 Apr 2025 5:06:31pm
    Author:  romal

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



const int MAX_INPUT_CHANNELS = 16;
const int MAX_BUFFER_SIZE = 9999; // remove this once i come up with a better buffer

const juce::String GATE_STATE_STRING = "GATE_STATE";
const juce::String GATE_ON_STRING = "GATE_ON";
const juce::String THRESHOLD_STRING = "THRESHOLD";
const juce::String ATTACK_STRING = "ATTACK";
const juce::String HOLD_STRING = "HOLD";
const juce::String RELEASE_STRING = "RELEASE";
const juce::String BASIC_GATE_STRING = "BASIC";
const juce::String RMS_GATE_STRING = "RMS";
const juce::String ATTACK_HOLD_RELEASE_GATE_STRING = "ATTACK/HOLD/RELEASE";
const float ATTACK_MIN = 0.0001;
const float RELEASE_MIN = 0.0001;
const float HOLD_MIN = 0.0001;


const juce::String DISTORTION_AMOUNT_STRING = "DISTORTION_AMOUNT";
const juce::String DISTORTION_ON_STRING = "DISTORTION_ON";
const juce::String DISTORTION_TYPE_STRING = "DISTORTION_TYPE";
const juce::String WAVE_RECTIFIER_STRING = "WAVE_RECTIFIER";
const juce::String BIT_CRUSHER_STRING = "BIT_CRUSHER";
const juce::String SOFT_CLIPPER_CUBIC_STRING = "SOFT_CLIPPER_CUBIC";
const juce::String SOFT_CLIPPER_ARCTAN_STRING = "SOFT_CLIPPER_ARCTAN";
const juce::String SLEW_LIMITER_STRING = "SLEW_LIMITER";

const juce::String FLANGER_DELAY_STRING = "FLANGER_DELAY_MS";
const juce::String FLANGER_MIX_STRING = "FLANGER_MIX";
const juce::String FLANGER_ON_STRING = "FLANGER_ON";
//const float DELAY_MAX = 0.01; 
const float DELAY_MAX = 1; //for debugging