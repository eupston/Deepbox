/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DeepboxAudioProcessorEditor::DeepboxAudioProcessorEditor (DeepboxAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    setSize (400, 300);
    addAndMakeVisible(mykickButton);
    addAndMakeVisible(mysnareButton);
    addAndMakeVisible(myhihatButton);
    addAndMakeVisible(midiDrag);
    mykickButton.onClick = [this] { processor.hitkick = true;};
    mysnareButton.onClick = [this] { processor.hitsnare = true;};
    myhihatButton.onClick = [this] { processor.hithihat = true;};
    
    sliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.treeState, "ONSET_THRESHOLD_ID", onset_threshold_slider);
    onset_threshold_slider.setSliderStyle(Slider::Slider::LinearVertical);
    onset_threshold_slider.setTextBoxStyle(Slider::TextBoxBelow, true, 100, 25);
    onset_threshold_slider.setRange(-48.0f, 0.0f);
    onset_threshold_slider.setValue(-15.0f);
    addAndMakeVisible(&onset_threshold_slider);
    
    
}

DeepboxAudioProcessorEditor::~DeepboxAudioProcessorEditor()
{
}

//==============================================================================
void DeepboxAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void DeepboxAudioProcessorEditor::resized()
{
    onset_threshold_slider.setBounds(250, 120, 50, 150);

    mykickButton.setBounds(100, 176, 150, 24);
    mysnareButton.setBounds(100, 206, 150, 24);
    myhihatButton.setBounds(100, 236, 150, 24);
    midiDrag.setBounds(0,0,200,100);
}
