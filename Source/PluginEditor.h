/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
//#include "Grain.h"

//==============================================================================
/**
*/
class TapePerformerAudioProcessorEditor  : public juce::AudioProcessorEditor,
public juce::FileDragAndDropTarget,
private juce::Timer
{
public:
    TapePerformerAudioProcessorEditor (TapePerformerAudioProcessor&);
    ~TapePerformerAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    
    bool isInterestedInFileDrag (const juce::StringArray& files) override;
    void filesDropped (const juce::StringArray& files, int x, int y) override;
    
    void paintIfFileLoaded(juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);
    void paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds);


private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.

    juce::TextButton mLoadButton { "Load" };
    
    juce::ComboBox modeSelector;
    juce::ComboBox keysAvailableSelector;
    
    juce::Slider positionSlider;
    juce::Slider durationSlider;
    juce::Slider spreadSlider;
    juce::Slider gainSlider;
    
    
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    std::unique_ptr<ComboBoxAttachment> modeAttachment;
    std::unique_ptr<ComboBoxAttachment> keysAvailableAttachment;
    
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> positionAttachment;
    std::unique_ptr<SliderAttachment> durationAttachment;
    std::unique_ptr<SliderAttachment> spreadAttachment;
    std::unique_ptr<SliderAttachment> gainAttachment;
    
    void setSliderParams(juce::Slider& slider);
    
    
    void timerCallback() override
    {
        repaint();
    }
    
    
    TapePerformerAudioProcessor& audioProcessor;
    
//    GrainSound& grainSound;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TapePerformerAudioProcessorEditor)
};
