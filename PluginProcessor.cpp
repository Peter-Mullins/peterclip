/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeterClip2AudioProcessor::PeterClip2AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), params(*this, nullptr, "parameters", createparameters()) // parameter constructor
#endif
{
}

PeterClip2AudioProcessor::~PeterClip2AudioProcessor()
{
}

//==============================================================================
const juce::String PeterClip2AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PeterClip2AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PeterClip2AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PeterClip2AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PeterClip2AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PeterClip2AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PeterClip2AudioProcessor::getCurrentProgram()
{
    return 0;
}

void PeterClip2AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PeterClip2AudioProcessor::getProgramName (int index)
{
    return {};
}

void PeterClip2AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PeterClip2AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void PeterClip2AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PeterClip2AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PeterClip2AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto gaincopy = params.getRawParameterValue("gain")->load();
    auto thresholdcopy = params.getRawParameterValue("threshold")->load();
    auto modeCopy = params.getParameter("mode")->getCurrentValueAsText();
    
    if (modeCopy == "Hard")
    {
        //hard clipping
        for (auto j = 0; j < buffer.getNumSamples(); ++j)
        {
            for (auto i = 0; i < mainInputOutput.getNumChannels(); ++i)
            {
                *mainInputOutput.getWritePointer(i, j) *= gaincopy;
                *mainInputOutput.getWritePointer(i, j) = 0.5 * (std::fabs(*mainInputOutput.getWritePointer(i, j) + thresholdcopy) - std::fabs(*mainInputOutput.getWritePointer(i, j) - thresholdcopy));
                
            }
        }
    }
    if (modeCopy == "Soft")
    {
        //soft clipping
        for (auto j = 0; j < buffer.getNumSamples(); ++j)
        {
            for (auto i = 0; i < mainInputOutput.getNumChannels(); ++i)
            {
                *mainInputOutput.getWritePointer(i, j) *= gaincopy;
                *mainInputOutput.getWritePointer(i, j) = 1.5f * *mainInputOutput.getWritePointer(i, j) - 0.5f * pow(*mainInputOutput.getWritePointer(i, j), 3.0f);
                    
            }
        }
    }
     
}

//==============================================================================
bool PeterClip2AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PeterClip2AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PeterClip2AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PeterClip2AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PeterClip2AudioProcessor();
}




juce::AudioProcessorValueTreeState::ParameterLayout PeterClip2AudioProcessor::createparameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    //these need the super annoying parameterID for audio unit (AU) compatibility
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"gain", 1}, "Gain", 1.0f, 3.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"threshold", 1}, "Threshold", 0.0f, 1.0f, 1.0f));
    
    //choice of distortion mode
    params.push_back(std::make_unique<juce::AudioParameterChoice> (juce::ParameterID{"mode", 1}, "Mode", juce::StringArray { "Hard", "Soft" }, 0));
    
    return { params.begin(), params.end()};
}
