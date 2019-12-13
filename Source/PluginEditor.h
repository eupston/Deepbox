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
    ImageButton mykickButton{"kick"};
    ImageButton mysnareButton{"snare"};
    ImageButton myhihatButton{"hihat"};
    ImageButton recordButton{"record"};
    ImageButton deepbox_text{"deepbox"};
    DragMidiFile midiDrag;
    DeepboxAudioProcessor& processor;
    Slider onset_threshold_slider;
    ScopedPointer <AudioProcessorValueTreeState::SliderAttachment> sliderAttach;
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DeepboxAudioProcessorEditor)
};
