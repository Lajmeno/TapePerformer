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
    
    
    
    modeAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "playMode", modeSelector);
    keysAvailableAttachment = std::make_unique<ComboBoxAttachment>(audioProcessor.apvts, "numKeys", keysAvailableSelector);
    
    
    positionAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "position", positionSlider);
    durationAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "duration", durationSlider);
    spreadAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "spread", spreadSlider);
    gainAttachment = std::make_unique<SliderAttachment>(audioProcessor.apvts, "gain", gainSlider);
    
    setSliderParams(positionSlider);
    setSliderParams(durationSlider);
    setSliderParams(spreadSlider);
    
    
}

TapePerformerAudioProcessorEditor::~TapePerformerAudioProcessorEditor()
{
}

//==============================================================================
void TapePerformerAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
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
//        auto a = audioProcessor.getNumSamplerSounds();
//        DBG("Hi");
//        g.fillAll(juce::Colours::red);
        g.drawText("Sound Loaded", parameterArea.getWidth() * 0.5, parameterArea.getHeight() + parameterArea.getHeight() * 0.5, 100, 20, juce::Justification::centred);
    }
    else
    {
        g.drawText("Parameter-Area", parameterArea.getWidth() * 0.5, parameterArea.getHeight() + parameterArea.getHeight() * 0.5, 100, 20, juce::Justification::centred);
    }
    
    
    
    
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
    

    auto audioLength = (float) audioProcessor.thumbnail.getTotalLength();
    
    //set first Line in another class - maybe red
    
    for (int i = 0; i < audioProcessor.mNumVoices; i++)
    {
                                                      
        if (auto voice = dynamic_cast<GrainVoice*>(audioProcessor.mSampler.getVoice(i)))
        {
//            audioProcessor.wavePlayPosition[i] = voice->getPosition();
            auto audioPosition = voice->getPosition() /  audioProcessor.getSampleRate();

            auto drawPosition = (audioPosition / audioLength) * (float) thumbnailBounds.getWidth() + (float) thumbnailBounds.getX();
            
            
            //to draw the very first playhead different

            if (auto sound = dynamic_cast<GrainSound*>(audioProcessor.mSampler.getSound(0).get()))
            {
                //get MidiNotenUmber that is trigerred if it is the root than draw red !!needs to be changed here!!!
                if ( audioProcessor.midiNoteForNormalPitch == 61 )
                {
                    g.setColour (juce::Colours::red);
                    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition, (float) thumbnailBounds.getBottom(), 1.5f);
                }
                else
                {
                    g.setColour (juce::Colours::black);
                    g.drawLine (drawPosition, (float) thumbnailBounds.getY(), drawPosition, (float) thumbnailBounds.getBottom(), 1.0f);
                }
            }
    
        }
    }
    
// highlight the portions of the sample that the keys are distributed on
    
    if (auto sound = dynamic_cast<GrainSound*>(audioProcessor.mSampler.getSound(0).get()))
    {
        auto numFragments = sound->getNumOfKeysAvailable();
        auto widthOfFragment = sound->getDurationParam() / audioProcessor.getSampleRate();
        auto initialXPosition = sound->getPositionsParam() / audioProcessor.getSampleRate();
        auto spreadParam = sound->getSpreadParam();
        
        
        for (int i = 0; i < numFragments; i++)
        {
            
            juce::Rectangle<int> fragmentBounds (initialXPosition / audioLength * thumbnailBounds.getWidth(), 0, widthOfFragment / audioLength * thumbnailBounds.getWidth(), thumbnailBounds.getHeight());

            initialXPosition = std::fmod(initialXPosition + (audioLength / numFragments) * spreadParam, audioLength);
            
            auto purpleHue = juce::Colours::plum.getHue();
            g.setColour (juce::Colour::fromHSV (purpleHue, 1.0f, 0.5f, 0.25f));
            g.fillRect (fragmentBounds);
            
            if( initialXPosition + widthOfFragment > audioLength)
            {
                juce::Rectangle<int> fragmentBounds (0, 0, ( (initialXPosition + widthOfFragment) - audioLength) / audioLength * thumbnailBounds.getWidth(), thumbnailBounds.getHeight());
                
//                auto purpleHue = juce::Colours::darkgrey.getHue();
                g.setColour (juce::Colour::fromHSV (purpleHue, 0.3f, 0.5f, 0.5f));
                g.fillRect (fragmentBounds);
                
            }
            
        }
        
    }

    


}

void TapePerformerAudioProcessorEditor::paintIfNoFileLoaded (juce::Graphics& g, const juce::Rectangle<int>& thumbnailBounds)
{
    g.setColour (juce::Colours::black);
    g.fillRect (thumbnailBounds);
    g.setColour (juce::Colours::white);
    g.drawFittedText ("Drag and Drop a File here", thumbnailBounds, juce::Justification::centred, 1);
}

void TapePerformerAudioProcessorEditor::setSliderParams(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 0);
    addAndMakeVisible(slider);
    
}
