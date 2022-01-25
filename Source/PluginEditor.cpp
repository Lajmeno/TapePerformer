/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "WaveDisplay.h"

//==============================================================================
TapePerformerAudioProcessorEditor::TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor& p)
    : AudioProcessorEditor (&p), waveDisplay(p), audioProcessor (p)
{

    addAndMakeVisible(waveDisplay);
    
    setSize (800, 300);
    
    
    modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "playMode", positionButton);
    keysAvailableAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.apvts, "numKeys", pitchButton);
    
    
    positionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    durationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "duration", durationSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    
    
    setSliderParams(positionSlider);
    setSliderParams(durationSlider);
    setSliderParams(spreadSlider);
    
    
    addAndMakeVisible (modeLabel);

    addAndMakeVisible (positionButton);
    addAndMakeVisible (pitchButton);
    positionButton  .onClick = [this] { updateToggleState (&positionButton,   "Position");   };
    pitchButton.onClick = [this] { updateToggleState (&pitchButton, "Pitch"); };

    positionButton  .setRadioGroupId (ModeButtons);
    pitchButton.setRadioGroupId (ModeButtons);

//    addAndMakeVisible (hobbiesLabel);
//
//    addAndMakeVisible (sportButton);
//    addAndMakeVisible (artButton);
//    addAndMakeVisible (filmButton);
//    sportButton.onClick = [this] { updateToggleState (&sportButton, "Sport"); };
//    artButton  .onClick = [this] { updateToggleState (&artButton,   "Art");   };
//    filmButton .onClick = [this] { updateToggleState (&filmButton,  "Film");  };
    
    
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{

    g.fillAll(juce::Colours::darkgrey);


}

void TapePerformerAudioProcessorEditor::resized()
{
    
    auto bounds = getLocalBounds();
    auto responseArea = bounds.removeFromTop(bounds.getHeight()* 0.5);
    
    waveDisplay.setBounds(responseArea);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    
    auto generalSettings = bounds.removeFromLeft(bounds.getWidth() * 0.15);
    auto positionArea = bounds.removeFromRight(bounds.getWidth() * 0.2);
    auto durationArea = bounds.removeFromRight(bounds.getWidth() * 0.2);
    auto spreadArea = bounds.removeFromRight(bounds.getWidth() * 0.2);
    
    modeLabel .setBounds (10, 10, generalSettings.getWidth() - 20, generalSettings.getHeight());
    positionButton  .setBounds (20, 40, generalSettings.getWidth() - 30, generalSettings.getHeight());
    pitchButton.setBounds (20, 70, generalSettings.getWidth() - 30, generalSettings.getHeight());
    
    modeSelector.setBounds(generalSettings.removeFromTop(generalSettings.getHeight() * 0.5));
    keysAvailableSelector.setBounds(generalSettings);

    positionSlider.setBounds(positionArea.removeFromTop(positionArea.getHeight() * 0.5));
    durationSlider.setBounds(durationArea.removeFromTop(durationArea.getHeight() * 0.5));
    spreadSlider.setBounds(spreadArea.removeFromTop(spreadArea.getHeight() * 0.5));
//    highCutSlopeSlider.setBounds(highCutArea);
    
//    peakFreqSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.33));
//    peakGainSlider.setBounds(bounds.removeFromTop(bounds.getHeight() * 0.5));
//    peakQualitySlider.setBounds(bounds);
    
    
}


void TapePerformerAudioProcessorEditor::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(slider);
    
}

void TapePerformerAudioProcessorEditor::updateToggleState (juce::Button* button, juce::String name)
{
    auto state = button->getToggleState();
    juce::String stateString = state ? "ON" : "OFF";

    juce::Logger::outputDebugString (name + " Button changed to " + stateString);
}
