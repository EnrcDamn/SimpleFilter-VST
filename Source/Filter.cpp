/*
  ==============================================================================

    Filter.cpp
    Created: 30 May 2022 11:36:01am
    Author:  Enrico Damiani

  ==============================================================================
*/

#include "Filter.h"

void Filter::setHighpass(bool highpass)
{
    this->highpass = highpass;
}

void Filter::setCutoffFrequency(float cutoffFrequency)
{
    this->cutoffFrequency = cutoffFrequency;
}

void Filter::setSamplingRate(float samplingRate)
{
    this->samplingRate = samplingRate;
}

void Filter::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    constexpr auto PI = juce::double_Pi;
    dnBuffer.resize(buffer.getNumChannels(), 0.f);

    // multiply one 
    // https://ccrma.stanford.edu/~jos/filters/Definition_Simplest_Low_Pass.html

    const auto sign = highpass ? -1.f : 1.f;
}