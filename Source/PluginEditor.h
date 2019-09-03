/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "DragMidiFile.hpp"

//==============================================================================
/**
*/
class DeepboxAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    DeepboxAudioProcessorEditor (DeepboxAudioProcessor&);
    ~DeepboxAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    TextButton mykickButton{"kick"};
    TextButton mysnareButton{"snare"};
    TextButton myhihatButton{"hihat"};
    DragMidiFile midiDrag;
    DeepboxAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeepboxAudioProcessorEditor)
};
