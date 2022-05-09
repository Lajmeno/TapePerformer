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
EnvelopeDisplay::EnvelopeDisplay() : envCurve()
{
    envCurve.setFrequency(40, 44100);
    envCurve.createWavetableEnv();
    startTimer(40);
    

}

EnvelopeDisplay::~EnvelopeDisplay()
{
}

void EnvelopeDisplay::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */
    

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    
    auto bounds = getLocalBounds();
    
    auto waveDisplayArea = bounds.removeFromTop(bounds.getHeight()* 0.75);
    

    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   // draw an outline around the component

    g.setColour (juce::Colours::white);
    
    
    
    for (int i = 0; i < 1024; i++){
        g.drawVerticalLine(envCurve.getNextSample(), waveDisplayArea.getHeight(), 0);
        
    }
    
    
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
