/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeterClip2AudioProcessorEditor::PeterClip2AudioProcessorEditor (PeterClip2AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (210, 300);
    
    //threshold slider parameters
    clipThreshSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    clipThreshSlider.setRange (0.0, 1.0, 0);
    clipThreshSlider.setTextBoxStyle (juce::Slider::NoTextBox, true, 90, 0);
    clipThreshSlider.setPopupDisplayEnabled (true, false, this);
    clipThreshSlider.setTextValueSuffix (" thresh");
    clipThreshSlider.setValue(0);
    
    //output gain slider parameters
    outputGainSlider.setSliderStyle (juce::Slider::LinearBarVertical);
    outputGainSlider.setRange (1.0, 18.0, 0); //it only makes it louder...
    outputGainSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 90, 0);
    outputGainSlider.setPopupDisplayEnabled (true, false, this);
    outputGainSlider.setTextValueSuffix (" gain");
    outputGainSlider.setValue(0);
    
    //creates the sliders
    addAndMakeVisible (&clipThreshSlider);
    addAndMakeVisible (&outputGainSlider);
    
    //listeners, so the values can alter pluginprocessor thread
    clipThreshSlider.addListener (this);
    outputGainSlider.addListener (this);
}

PeterClip2AudioProcessorEditor::~PeterClip2AudioProcessorEditor()
{
}

//==============================================================================
void PeterClip2AudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (18.0f);
    g.drawFittedText ("PeterClip", 0, 0, getWidth(), 30, juce::Justification::centred, 1);
}

void PeterClip2AudioProcessorEditor::resized()
{
    outputGainSlider.setBounds (150, 30, 20, getHeight() - 60);
    clipThreshSlider.setBounds (40, 30, 20, getHeight() - 60);
}

void PeterClip2AudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
    audioProcessor.threshold = clipThreshSlider.getValue();
    audioProcessor.gain = outputGainSlider.getValue();
}
