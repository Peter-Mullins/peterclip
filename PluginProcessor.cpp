#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeterClip3AudioProcessor::PeterClip3AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), params(*this, nullptr, "parameters", createparameters()),
oversampler(PeterClip3AudioProcessor::getTotalNumInputChannels(), osMode, juce::dsp::Oversampling<float>::FilterType::filterHalfBandFIREquiripple, true, true)

#endif
{
}

PeterClip3AudioProcessor::~PeterClip3AudioProcessor()
{
}

//==============================================================================
const juce::String PeterClip3AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PeterClip3AudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PeterClip3AudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PeterClip3AudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PeterClip3AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PeterClip3AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PeterClip3AudioProcessor::getCurrentProgram()
{
    return 0;
}

void PeterClip3AudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PeterClip3AudioProcessor::getProgramName (int index)
{
    return {};
}

void PeterClip3AudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PeterClip3AudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    
    oversampler.initProcessing(samplesPerBlock);
}

void PeterClip3AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PeterClip3AudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void PeterClip3AudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    
    
    //selects algorithm there is a better way to do this and use the APVTS I am sure.
    currentMode = params.getParameter("mode")->getCurrentValueAsText();
    if (currentMode.equalsIgnoreCase("Hard"))
    {
        algo = &PeterClip3AudioProcessor::algoHard;
    }
    if (currentMode.equalsIgnoreCase("Cubic"))
    {
        algo = &PeterClip3AudioProcessor::algoCubic;
    }
    if (currentMode.equalsIgnoreCase("Arctan"))
    {
        algo = &PeterClip3AudioProcessor::algoArcTan;
    }
    if (currentMode.equalsIgnoreCase("FuzzExp"))
    {
        algo = &PeterClip3AudioProcessor::algoFuzzExp;
    }
    if (currentMode.equalsIgnoreCase("Sigmoid"))
    {
        algo = &PeterClip3AudioProcessor::algoSigmoid;
    }
    if (currentMode.equalsIgnoreCase("ArcTanSQRT"))
    {
        algo = &PeterClip3AudioProcessor::algoArcTanSQRT;
    }
    if (currentMode.equalsIgnoreCase("SQRT"))
    {
        algo = &PeterClip3AudioProcessor::algoSQRT;
    }
    if (currentMode.equalsIgnoreCase("SquareSign"))
    {
        algo = &PeterClip3AudioProcessor::algoSquareSign;
    }
    
    auto mainInputOutput = getBusBuffer (buffer, true, 0);
    auto gaincopy = params.getRawParameterValue("gain")->load();
    auto thresholdcopy = params.getRawParameterValue("threshold")->load();
    
    //I take it that to use the juce::dsp methods, this how buffers are handled...
    juce::dsp::AudioBlock<float> block (buffer);
    juce::dsp::AudioBlock<float> upSampleBlock (buffer);
    
    //oversampling
    osMode = params.getParameter("os")->getValue();
    if (osMode > 0)
    {
        upSampleBlock = oversampler.processSamplesUp(block);
        for (auto j = 0; j < upSampleBlock.getNumSamples(); ++j)
        {
            for (auto i = 0; i < upSampleBlock.getNumChannels(); ++i)
            {
                float* data = upSampleBlock.getChannelPointer(i);
                //nasty looking member function pointer
                data[j] = (this->*algo)(data[j], gaincopy, thresholdcopy);
                
            }
        }
        oversampler.processSamplesDown(block);
    }
    else
    {
        //non-oversampled...I am repeating myself here, and will need to refactor to make this "correct" with future version
        for (auto j = 0; j < block.getNumSamples(); ++j)
        {
            for (auto i = 0; i < block.getNumChannels(); ++i)
            {
                float* data = block.getChannelPointer(i);
                
                data[j] = (this->*algo)(data[j], gaincopy, thresholdcopy);
            }
        }
    }
    
}

//==============================================================================
bool PeterClip3AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PeterClip3AudioProcessor::createEditor()
{
    return new juce::GenericAudioProcessorEditor (*this);
}

//==============================================================================
void PeterClip3AudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PeterClip3AudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PeterClip3AudioProcessor();
}




juce::AudioProcessorValueTreeState::ParameterLayout PeterClip3AudioProcessor::createparameters()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;
    //these need the "parameterID" for audio unit (AU) compatibility
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"gain", 1}, "Gain", 1.0f, 5.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> (juce::ParameterID{"threshold", 1}, "Threshold", 0.0f, 10.0f, 10.0f));
    
    //choice of distortion mode
    params.push_back(std::make_unique<juce::AudioParameterChoice> (juce::ParameterID{"mode", 1}, "Mode", juce::StringArray { "Hard", "Cubic", "ArcTan", "FuzzExp", "Sigmoid", "ArctanSQRT", "SQRT", "SquareSign" }, 0));
    
    //oversampling mode, an integer which will oversample at 2^n
    params.push_back(std::make_unique<juce::AudioParameterInt> (juce::ParameterID{"os", 1}, "Oversampling", 0, 2, 0));
    
    return { params.begin(), params.end()};
}

//different algorithms, many from Will Pirkle's Audio Effects book

//simple hardclip at threshold
float PeterClip3AudioProcessor::algoHard(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = 0.5 * (std::fabs(x + (thresholdCopy/10.0)) - (std::fabs(x - (thresholdCopy/10.0))));
    
    return x;
    
}

//cubic
float PeterClip3AudioProcessor::algoCubic(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = 1.5f * x - 0.5f * pow(x, 3.0f);
    
    return x;
}

//arctangent
float PeterClip3AudioProcessor::algoArcTan(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = atan(thresholdCopy*x)/atan(thresholdCopy);
    
    return x;
}

//"fuzz exponential" from will pirkle's book
float PeterClip3AudioProcessor::algoFuzzExp(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = copysignf(1.0, x) * (1.0 - expf(fabs(-1.0 * thresholdCopy * x)))/(1.0 - expf(-1.0 * thresholdCopy));
    
    return x;
}

//sigmoid also from Pirkle
float PeterClip3AudioProcessor::algoSigmoid(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = 2.0 * (1.0 / (1 + exp(-1.0 * (thresholdCopy + 2.0)* x))) - 1.0;
    
    return x;
}

//from Pirkle, a more exotic distortion
float PeterClip3AudioProcessor::algoArcTanSQRT(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = (2.5 * atan(0.9 * x)) + (2.5 * powf((1.0f - (0.9f * powf(x, 2.0f))), 0.5f)) - 2.5;
    
    return x;
}

//square root curve, but signed to not rectify
float PeterClip3AudioProcessor::algoSQRT(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = copysignf(1.0, x)*powf(fabsf(x), 0.5f);
    
    return x;
}

//"mirror image" of square root curve
float PeterClip3AudioProcessor::algoSquareSign(float x, float gainCopy, float thresholdCopy)
{
    x *= gainCopy;
    x = copysignf(1.0, x)*powf(x, 2.0);
    
    return x;
}


float PeterClip3AudioProcessor::*algo(float x, float gainCopy, float, float thresholdCopy);
