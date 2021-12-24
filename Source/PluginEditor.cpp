/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapePerformerAudioProcessorEditor::TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    mLoadButton.onClick = [&]() { audioProcessor.loadFile(); };
    addAndMakeVisible(mLoadButton);
    
    setSize (400, 300);
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::black);
    
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
    if (audioProcessor.getNumSamplerSounds() > 0)
    {
        g.fillAll(juce::Colours::red);
        g.drawText("Sound Loaded", getWidth() / 2 - 50, getHeight() / 2 - 10 , 100, 20, juce::Justification::centred);
    }
    else
    {
        g.drawText("Load Sound", getWidth() / 2 - 50, getHeight() / 2 - 10 , 100, 20, juce::Justification::centred);
    }
    
}

void TapePerformerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    //mLoadButton.setBounds(getWidth() / 2 - 50, getHeight() / 2 - 50, 100, 100);
}

bool TapePerformerAudioProcessorEditor::isInterestedInFileDrag(const juce::StringArray &files)
{
    for ( auto file:files)
    {
        if (file.contains(".wav")||file.contains(".mp3")||file.contains(".aif"))
        {
            return true;
        }
    }
    return false;
}


void TapePerformerAudioProcessorEditor::filesDropped(const juce::StringArray &files, int x, int y)
{
    for ( auto file:files)
    {
        if (isInterestedInFileDrag(files))
        {
            audioProcessor.loadFile (file);
        }
    }
    repaint();
}
