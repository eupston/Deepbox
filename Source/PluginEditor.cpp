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
    mykickButton.setBounds(100, 176, 150, 24);
    mysnareButton.setBounds(100, 206, 150, 24);
    myhihatButton.setBounds(100, 236, 150, 24);
    midiDrag.setBounds(0,0,200,100);
}
