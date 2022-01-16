/*
  ==============================================================================

    Grain.h
    Created: 4 Jan 2022 12:25:01pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>



class GrainSound : public juce::SynthesiserSound
{
public:
    GrainSound (const juce::String& name,
                juce::AudioFormatReader& source,
                  const juce::BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds);

    /** Destructor. */
    ~GrainSound() override;
    
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    bool pitchModeParam = false;
  
    float transpositionParam = 60.0;   //midiRoot
    double positionParam = 0;
    double durationParam = 22050;
    int numOfKeysAvailable = 12;
    
    
    
    
private:
    friend class GrainVoice;
    
    juce::String name;
    std::unique_ptr<juce::AudioBuffer<float>> data;
    double sourceSampleRate;
    juce::BigInteger midiNotes;
    int length = 0, midiRootNote = 0;
    
    juce::ADSR::Parameters params;
    
//    JUCE_LEAK_DETECTOR (GrainSound)
};


class GrainVoice : public juce::SynthesiserVoice
{
public:
    /** Creates a SamplerVoice. */
    GrainVoice();

    /** Destructor. */
    ~GrainVoice() override;

    //==============================================================================
    bool canPlaySound (juce::SynthesiserSound*) override;

    void startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound*, int pitchWheel) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
//    bool isKeyDown() const noexcept                             { return keyIsDown; }

    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;

    void renderNextBlock (juce::AudioBuffer<float>&, int startSample, int numSamples) override;
    using juce::SynthesiserVoice::renderNextBlock;
    
    std::atomic<bool>& misNotePlayed() { return isNotePlayed; }
    std::atomic<double>& getSamplePlayPosition() { return samplePlayPosition; }
    
    
private:
    double sampleRate = 0;
    bool keyIsDown = false;
    
    double startPosition = 0;
    
    double pitchRatio = 0;
    double sourceSamplePosition = 0;
    double numPlayedSamples = 0;
    float lgain = 0, rgain = 0;
    
    
    //Implement the playposition to display on surface
    std::atomic<double> samplePlayPosition = 0;
    std::atomic<bool> isNotePlayed = false;
    
    

    juce::ADSR adsr;
    
//    JUCE_LEAK_DETECTOR (GrainVoice)
};


