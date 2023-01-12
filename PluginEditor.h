#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PeterClip3AudioProcessorEditor  : public juce::AudioProcessorEditor,
                                        private juce::Slider::Listener
{
public:
    PeterClip3AudioProcessorEditor (PeterClip3AudioProcessor&);
    ~PeterClip3AudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    
    PeterClip3AudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PeterClip3AudioProcessorEditor)
};
