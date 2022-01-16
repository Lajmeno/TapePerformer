/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
TapePerformerAudioProcessor::TapePerformerAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
,thumbnailCache (2),                            // [4]
thumbnail (512, mFormatManager, thumbnailCache)
{
    
    mFormatManager.registerBasicFormats();
    
//    transportSource.addChangeListener (this);
    
    for (int i = 0; i < mNumVoices; i++)
    {
        mSampler.addVoice(new GrainVoice());
//        wavePlayPosition = 0;
    }
}
 
TapePerformerAudioProcessor::~TapePerformerAudioProcessor()
{
    mFormatReader = nullptr;
//    wavePlayPosition = 0;
}

//==============================================================================
const juce::String TapePerformerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TapePerformerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TapePerformerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TapePerformerAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TapePerformerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TapePerformerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TapePerformerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TapePerformerAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String TapePerformerAudioProcessor::getProgramName (int index)
{
    return {};
}

void TapePerformerAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void TapePerformerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    mSampler.setCurrentPlaybackSampleRate(sampleRate);
    
//    wavePlayPosition = 0;
    
}

void TapePerformerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TapePerformerAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void TapePerformerAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());
    
    mSampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    
    
//    std::floor(buffer.getNumSamples() * 0.5f)
    
//    if (mSampler.getNumSounds() != 0)
//    {
//        wavePlayPosition += getSampleRate();
//    }

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    /*
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
     */
}

//==============================================================================
bool TapePerformerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* TapePerformerAudioProcessor::createEditor()
{
    return new TapePerformerAudioProcessorEditor (*this);
}

//==============================================================================
void TapePerformerAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TapePerformerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}


void TapePerformerAudioProcessor::loadFile()
{
    mSampler.clearSounds();
    
    //bug: when closing Filechooser without choosing a file - no file is load
    //need to keep the old file in that situation
    
    chooser = std::make_unique<juce::FileChooser> ("Select a Wave file shorter than 10 minutes to play...",
                                                   juce::File{},
                                                   "*.wav");
    auto chooserFlags = juce::FileBrowserComponent::openMode
                      | juce::FileBrowserComponent::canSelectFiles;

    chooser->launchAsync (chooserFlags, [this] (const juce::FileChooser& fc)
    {
        auto file = fc.getResult();

        if (file == juce::File{})
            return;
        std::unique_ptr<juce::AudioFormatReader> reader (mFormatManager.createReaderFor (file)); // [2]
//        mFormatReader = mFormatManager.createReaderFor(file);
 
        if (reader.get() != nullptr)
        {
            mFormatReader = mFormatManager.createReaderFor(file);
            thumbnail.setSource (new juce::FileInputSource (file));
            
            juce::BigInteger range;
            range.setRange(0, 127, true);
            mSampler.addSound(new GrainSound("Sample", *mFormatReader, range, 60, 0, 0, 180));
        }
    });
    
}
 
 
void TapePerformerAudioProcessor::loadFile(const juce::String &path)
{
    mSampler.clearSounds();
    auto file = juce::File (path);
    mFormatReader = mFormatManager.createReaderFor(file);
    thumbnail.setSource (new juce::FileInputSource (file));
    
    juce::BigInteger range;
    range.setRange(0, 127, true);
    
    mSampler.addSound(new GrainSound("Sample", *mFormatReader, range, 60, 0, 0, 180));
    
//    wavePlayPosition = 0;
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TapePerformerAudioProcessor();
}
