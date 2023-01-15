#pragma once

#include <JuceHeader.h>

//==============================================================================

class PeterClip3AudioProcessor  : public juce::AudioProcessor
                            #if JucePlugin_Enable_ARA
                             , public juce::AudioProcessorARAExtension
                            #endif
{
public:
    //==============================================================================
    PeterClip3AudioProcessor();
    ~PeterClip3AudioProcessor() override;
    

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    //in case I need to pass "params" to the editor in a future version
    juce::AudioProcessorValueTreeState params;
    
    //function pointer to current distortion algorithm
    float (PeterClip3AudioProcessor::*algo)(float, float, float);
    
    //distortion algorithms
    float algoHard(float x, float gainCopy, float thresholdCopy);
    float algoCubic(float x, float gainCopy, float thresholdCopy);
    float algoArcTan(float x, float gainCopy, float thresholdCopy);
    float algoFuzzExp(float x, float gainCopy, float thresholdCopy);
    float algoSigmoid(float x, float gainCopy, float thresholdCopy);
    float algoArcTanSQRT(float x, float gainCopy, float thresholdCopy);
    float algoSQRT(float x, float gainCopy, float thresholdCopy);
    float algoSquareSign(float x, float gainCopy, float thresholdCopy);
private:
    //this function creates the parameter objects
    juce::AudioProcessorValueTreeState::ParameterLayout createparameters();
    
    //string to pick the algorithm, annoying way this AVTS functions
    juce::String currentMode;
    int osMode = 0;
    
    juce::dsp::Oversampling<float> oversampler;

    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PeterClip3AudioProcessor)
};
