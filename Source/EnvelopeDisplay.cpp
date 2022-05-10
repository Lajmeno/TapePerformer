/*
  ==============================================================================

    EnvelopeDisplay.cpp
    Created: 29 Jan 2022 2:15:43pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#include <JuceHeader.h>
#include "EnvelopeDisplay.h"

//==============================================================================
EnvelopeDisplay::EnvelopeDisplay() : envCurve(), thumbnailCache(2), thumbnail(16, mFormatManager, thumbnailCache)
{
    envCurve.createWavetableEnv();
    envCurve.setFrequency(8, 44100);
    startTimer(40);
    
    mFormatManager.registerBasicFormats();

    buffer.setSize(1, 44100);
    setThumbnailSource();
}

EnvelopeDisplay::~EnvelopeDisplay()
{
}

void EnvelopeDisplay::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    
    auto bounds = getLocalBounds();
    
    auto waveDisplayArea = bounds.removeFromTop(bounds.getHeight()* 0.75);
    

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    
    //thumbnail.drawChannels (g, waveDisplayArea, 0.0, thumbnail.getTotalLength(), 1.0f);
    
    drawWaveform(g, waveDisplayArea);
    
    
    
    /*
    for (int i = 8; i < 1024; i++){
        //int x_pos = (waveDisplayArea.getWidth() / 1024.0f) * i;
        g.drawVerticalLine((int)((waveDisplayArea.getWidth() / 1024.0f) * i), 0.0f, (envCurve.getNextSample() + 1) * (waveDisplayArea.getHeight() / 2));

        
    }
     */
    
    
    /*
    g.setFont (14.0f);
    g.drawText ("EnvelopeDisplay", getLocalBounds(),
                juce::Justification::centred, true);   // draw some placeholder text
     */
}

void EnvelopeDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void EnvelopeDisplay::setThumbnailSource() {
    
    float* out = buffer.getWritePointer(0);
    
    for (int i = 0; i < 44100; i++)
    {
        buffer.addSample(0, i, envCurve.getNextSample());
        //*out++ += envCurve.getNextSample();
    }
    thumbnail.reset(1, 44100, buffer.getNumSamples());
    thumbnail.addBlock(0, buffer, 0, buffer.getNumSamples());
}

void EnvelopeDisplay::drawWaveform(juce::Graphics& g, const juce::Rectangle<int>& waveDisplayArea)
{
    
    for (int i = 0; i < 1024; i++)
    {
        g.drawVerticalLine((int)((waveDisplayArea.getWidth() / 1024.0f) * i), 0.0f, (envCurve.getNextSample() + 1) * (waveDisplayArea.getHeight() / 2));
    }
    
}
