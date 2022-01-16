/*
  ==============================================================================

    Grain.cpp
    Created: 7 Jan 2022 11:10:46pm
    Author:  Abdullah Ismailogullari

  ==============================================================================
*/
#include "Grain.h"



GrainSound::GrainSound (const juce::String& soundName,
                            juce::AudioFormatReader& source,
                            const juce::BigInteger& notes,
                            int midiNoteForNormalPitch,
                            double attackTimeSecs,
                            double releaseTimeSecs,
                            double maxSampleLengthSeconds)
    : name (soundName),
      sourceSampleRate (source.sampleRate),
      midiNotes (notes),
      midiRootNote (midiNoteForNormalPitch)
{
    if (sourceSampleRate > 0 && source.lengthInSamples > 0)
    {
        length = juce::jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));

        data.reset (new juce::AudioBuffer<float> (juce::jmin (2, (int) source.numChannels), length + 4));

        source.read (data.get(), 0, length + 4, 0, true, true);

        params.attack  = static_cast<float> (attackTimeSecs);
        params.release = static_cast<float> (releaseTimeSecs);
    }
}

GrainSound::~GrainSound()
{
}

bool GrainSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes[midiNoteNumber];
}

bool GrainSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

//==============================================================================
GrainVoice::GrainVoice() {}
GrainVoice::~GrainVoice() {}

bool GrainVoice::canPlaySound (juce::SynthesiserSound* sound)
{
    return dynamic_cast<const GrainSound*> (sound) != nullptr;
}

void GrainVoice::startNote (int midiNoteNumber, float velocity, juce::SynthesiserSound* s, int /*currentPitchWheelPosition*/)
{
    if (auto* sound = dynamic_cast<const GrainSound*> (s))
    {
        
        if(sound->pitchModeParam)
        {
            
            pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            
            sourceSamplePosition = sound->positionParam;
        }
        else
        {
            pitchRatio = std::pow (2.0, (sound->transpositionParam - sound->midiRootNote) / 12.0) * sound->sourceSampleRate / getSampleRate();
            
            sourceSamplePosition = std::fmod((sound->positionParam +  float(midiNoteNumber % sound->numOfKeysAvailable) / float(sound->numOfKeysAvailable) * sound->length), sound->length);
            
        }
        startPosition = sourceSamplePosition;
//        (sound->pitchModeParam) ? ( pitchRatio = std::pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
//                        * sound->sourceSampleRate / getSampleRate())
//                            : pitchRatio = 1;

//        sourceSamplePosition = 0.0;
        //Change position here by taking the length of the whole sample and then
        //change in relation to midiNoteNuber
//        sourceSamplePosition = 44100;
        numPlayedSamples = 0;
        lgain = velocity;
        rgain = velocity;

        adsr.setSampleRate (sound->sourceSampleRate);
        adsr.setParameters (sound->params);

        adsr.noteOn();
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void GrainVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        adsr.noteOff();
    }
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void GrainVoice::pitchWheelMoved (int /*newValue*/) {}
void GrainVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

//==============================================================================
void GrainVoice::renderNextBlock (juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<GrainSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data = *playingSound->data;
//        auto& data = outputBuffer;
        const float* const inL = data.getReadPointer (0);
        const float* const inR = data.getNumChannels() > 1 ? data.getReadPointer (1) : nullptr;

        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        
        //this for Thumbnai to show playhead
        if (isKeyDown())
        {
            samplePlayPosition = (int) sourceSamplePosition;
            isNotePlayed = true;
        }
        else
        {
            samplePlayPosition = playingSound->positionParam;
            isNotePlayed = false;
        }
        

        while (--numSamples >= 0)
        {
            auto pos = (int) sourceSamplePosition;
            auto alpha = (float) (sourceSamplePosition - pos);
            auto invAlpha = 1.0f - alpha;

            // just using a very simple linear interpolation here..
            float l = (inL[pos] * invAlpha + inL[pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR[pos] * invAlpha + inR[pos + 1] * alpha)
                                       : l;

            auto envelopeValue = adsr.getNextSample();

            l *= lgain * envelopeValue;
            r *= rgain * envelopeValue;

            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }
            
            //Adding Pitch Ratio = Pitch Mode
//            (pitchModeParam == true) ? (sourceSamplePosition += pitchRatio) : (sourceSamplePosition += 1.0);

            sourceSamplePosition += pitchRatio;
            sourceSamplePosition = std::fmod(sourceSamplePosition, playingSound->length);
            
            numPlayedSamples += pitchRatio;
            
            if (!isKeyDown())// && numPlayedSamples > durationParam )
            {
                DBG("HI");
                stopNote (0.0f, false);
                break;
            }
            else if (numPlayedSamples > playingSound->durationParam)
            {
                numPlayedSamples = 0;
//                sourceSamplePosition = playingSound->positionParam;
                sourceSamplePosition = startPosition;
            }
            
//            else if (numPlayedSamples + playingSound->positionParam > playingSound->length) //Problem when numPlayedSound becomes zero but is used to figure out if the duration is completed
//            {
//                numPlayedSamples = 0;
//                sourceSamplePosition = 0;
////                playingSound->length *= 2;
//            }
        }
    }
}
