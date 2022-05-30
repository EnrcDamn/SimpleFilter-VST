/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleFilterAudioProcessor::SimpleFilterAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
}

SimpleFilterAudioProcessor::~SimpleFilterAudioProcessor()
{
}

//==============================================================================
const juce::String SimpleFilterAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleFilterAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFilterAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFilterAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleFilterAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleFilterAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleFilterAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleFilterAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleFilterAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleFilterAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleFilterAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    juce::dsp::ProcessSpec spec;

    spec.maximumBlockSize = samplesPerBlock;

    spec.numChannels = 1;

    spec.sampleRate = sampleRate;

    leftChain.prepare(spec);
    rightChain.prepare(spec);
}

void SimpleFilterAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleFilterAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void SimpleFilterAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::dsp::AudioBlock<float> block(buffer);

    auto leftBlock = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftContext(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightContext(rightBlock);

    leftChain.process(leftContext);
    rightChain.process(rightContext);

}

//==============================================================================
bool SimpleFilterAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleFilterAudioProcessor::createEditor()
{
    // return new SimpleFilterAudioProcessorEditor (*this);
    return new juce::GenericAudioProcessorEditor(*this);
}

//==============================================================================
void SimpleFilterAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void SimpleFilterAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

ChainSettings addChainSettings(juce::AudioProcessorValueTreeState& avpts)
{
    ChainSettings settings;

    return settings;
}

juce::AudioProcessorValueTreeState::ParameterLayout
    SimpleFilterAudioProcessor::createParameterLayout()
{
    // spec: 3 bands
    // low, high, parametric / peak
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Low Cut Freq",
        "Low Cut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        20.f // default value
        )
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "High Cut Freq",
        "High Cut Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        20000.f // default value
        )
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Freq",
        "Peak Freq",
        juce::NormalisableRange<float>(20.f, 20000.f, 1.f, 1.f),
        750.f // default value
        )
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Gain",
        "Peak Gain",
        juce::NormalisableRange<float>(-24.f, 24.f, 0.1f, 1.f),
        0.0f // default value
        )
    );

    layout.add(std::make_unique<juce::AudioParameterFloat>(
        "Peak Quality",
        "Peak Quality",
        juce::NormalisableRange<float>(0.1f, 10.f, 0.05f, 1.f),
        1.f // default value
        )
    );

    juce::StringArray stringArray;
    for (int i = 0; i < 4; i++)
    {
        juce::String str;
        str << (12 + i*12);
        str << " db/Oct";
        stringArray.add(str);
    }

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "Low Cut Slope",
        "Low Cut Slope",
        stringArray,
        0 // default value
        )
    );

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        "high Cut Slope",
        "High Cut Slope",
        stringArray,
        0 // default value
        )
    );

    return layout;
    // low, high: controllable freq cutoff, slope
    // peak: controllable center freq, gain, quality 
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleFilterAudioProcessor();
}
