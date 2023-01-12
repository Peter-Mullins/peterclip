#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PeterClip3AudioProcessorEditor::PeterClip3AudioProcessorEditor (PeterClip3AudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    //Someday this will have the foleyGUI implemented for a neat UI
}

PeterClip3AudioProcessorEditor::~PeterClip3AudioProcessorEditor()
{
}

//==============================================================================
void PeterClip3AudioProcessorEditor::paint (juce::Graphics& g)
{
}

void PeterClip3AudioProcessorEditor::resized()
{
}

//
