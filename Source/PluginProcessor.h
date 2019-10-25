/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "AudioFeatureExtractor.hpp"
#include <fdeep/fdeep.hpp>
#include "BTrack/BTrack.h"

#include "essentia/algorithmfactory.h"
#include "essentia/essentiamath.h"
#include "essentia/pool.h"

#include "beat_detection/Source/Core/OnsetClassification.h"
#include "AudioLiveScrollingDisplay.h"

using namespace essentia;
using namespace essentia::standard;


//==============================================================================
/**
*/
class DeepboxAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    DeepboxAudioProcessor();
    ~DeepboxAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    void recordMidi (bool isRecording);
    //Initialise the synth object
    void initialiseSynth();
    vector<MidiMessage> triggerKickDrum(MidiBuffer& midiMessages, double msPerTick) const;
    vector<MidiMessage> triggerSnareDrum(MidiBuffer& midiMessages, double msPerTick) const;
    vector<MidiMessage> triggerHihatDrum(MidiBuffer& midiMessages, double msPerTick) const;
    bool hitkick = false;
    bool hitsnare = false;
    bool hithihat = false;
    AudioProcessorValueTreeState treeState;
    LiveScrollingAudioDisplay liveAudioScroller;

private:
    const int kickNoteNumber = 36;
    const int snareNoteNumber = 38;
    const int hihatNoteNumber = 40;
    MidiOutput *midiOut;
    MidiOutput *midiIn;
    Synthesiser drumSynth;
    StringArray mididevices;
    fdeep::model mymodel{fdeep::load_model("/Volumes/Macintosh HD/Users/macuser/Desktop/MyCode/myjuce/Deepbox/Source/resources/models/my_fdeep_model.json")};
    OnsetClassification my_onset_detector;
    MidiMessageSequence mms;
    int tempo;
    int microsecondsPerQuarter;
    double msPerTick;
    double startTime;
    AudioPlayHead* playHead;
    AudioPlayHead::CurrentPositionInfo currentPositionInfo;
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeepboxAudioProcessor)
};
