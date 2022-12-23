/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PeterClip2AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::Slider::Listener
{
public:
    PeterClip2AudioProcessorEditor (PeterClip2AudioProcessor&);
    ~PeterClip2AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    void sliderValueChanged (juce::Slider* slider) override;
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PeterClip2AudioProcessor& audioProcessor;
    
    juce::Slider clipThreshSlider;
    juce::Slider outputGainSlider;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PeterClip2AudioProcessorEditor)
};
