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
    //this is all empty because the plugin now uses the generic editor
    //which is how all my stuff will be until I feel like messing with FoleyGUI thingy
}

PeterClip2AudioProcessorEditor::~PeterClip2AudioProcessorEditor()
{
}

//==============================================================================
void PeterClip2AudioProcessorEditor::paint (juce::Graphics& g)
{
}

void PeterClip2AudioProcessorEditor::resized()
{
}

//
