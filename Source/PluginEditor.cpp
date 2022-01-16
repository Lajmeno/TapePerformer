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
//    audioProcessor.transportSource.addChangeListener (this);
    addAndMakeVisible(mLoadButton);
    
    setSize (800, 300);
    
    startTimer(40);
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::darkgrey);
    
    auto bounds = getLocalBounds();
    auto waveFileArea = bounds.removeFromTop(bounds.getHeight()* 0.5);
    
//    g.setColour (juce::Colours::black);
//    g.fillRect (waveFileArea);
    
    juce::Rectangle<int> thumbnailBounds (0, 0, waveFileArea.getWidth(), waveFileArea.getHeight());
    
    
    
    if (audioProcessor.thumbnail.getNumChannels() == 0)
    {
        paintIfNoFileLoaded (g, thumbnailBounds);
    }
    else
    {
//        auto& sound = grainSound.positionParam;
        paintIfFileLoaded (g, thumbnailBounds);
    }
    
    mLoadButton.setBounds(waveFileArea.getWidth() * 0.95, waveFileArea.getHeight() * 0.02, 40, 20);
    
    //responseCurveComponent.setBounds(responseArea);
    
    auto parameterArea = bounds.removeFromTop(bounds.getHeight());
//    DBG("out");
    g.setColour(juce::Colours::white);
    g.setFont(15.0f);
    
    if (audioProcessor.getNumSamplerSounds() > 0)
    {
//        g.fillAll(juce::Colours::red);
        g.drawText("Sound Loaded", parameterArea.getWidth() * 0.5, parameterArea.getHeight() + parameterArea.getHeight() * 0.5, 100, 20, juce::Justification::centred);
    }
    else
    {
        g.drawText("Parameter-Area", parameterArea.getWidth() * 0.5, parameterArea.getHeight() + parameterArea.getHeight() * 0.5, 100, 20, juce::Justification::centred);
    }
    
    
    
    //here
    
}

void TapePerformerAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    
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

void TapePerformerAudioProcessorEditor::paintIfFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::darkorange);
    g.fillRect (thumbnailBounds);

    g.setColour (juce::Colours::lightseagreen);  // [8]
    


    audioProcessor.thumbnail.drawChannels (g,                                      // [9]
                            thumbnailBounds,
                            0.0,                                    // start time
                            audioProcessor.thumbnail.getTotalLength(),             // end time
                            1.0f);                                  // vertical zoom
    

//    auto audioLength = (float) audioProcessor.thumbnail.getTotalLength();
//    g.setColour (juce::Colours::black);
//    auto audioPosition = grainSound.positionParam;
//    
//    auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth() + (float) thumbnailBounds.getX();
//    
//    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
//                    (float) thumbnailBounds.getBottom(), 2.0f);
//                                                        // [13]
        
    
//    auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth()
//                        + (float) thumbnailBounds.getX();                                // [13]
//    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition,
//                (float) thumbnailBounds.getBottom(), 2.0f);
}

void TapePerformerAudioProcessorEditor::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::black);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("Drag and Drop a File here", thumbnailBounds, juce::Justification::centred, 1);
}
