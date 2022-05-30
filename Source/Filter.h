/*
  ==============================================================================

    Filter.h
    Created: 30 May 2022 11:36:01am
    Author:  Enrico Damiani

  ==============================================================================
*/
#pragma once

#include <vector>
#include <JuceHeader.h>

class Filter 
{
public:
    void setHighpass(bool highpass);
    void setCutoffFrequency(float cutoffFrequency);
    void setSamplingRate(float samplingRate);
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&);

private:
    bool highpass;
    float cutoffFrequency;
    float samplingRate;
    std::vector<float> dnBuffer;

};