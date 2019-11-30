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
    setSize (400, 350);
    
    auto kickImg = ImageCache::getFromMemory(BinaryData::kick_drum_icon_png, BinaryData::kick_drum_icon_pngSize);
    auto snareImg = ImageCache::getFromMemory(BinaryData::snare_icon_png, BinaryData::snare_icon_pngSize);
    auto hihatImg = ImageCache::getFromMemory(BinaryData::hihat_icon_png, BinaryData::snare_icon_pngSize);

    mykickButton.setImages(false, true, true, kickImg, 1.0f, Colours::transparentBlack, kickImg, 0.5f, Colours::aquamarine, kickImg, 0.5f, Colours::blueviolet);
    mysnareButton.setImages(false, true, true, snareImg, 1.0f, Colours::transparentBlack, snareImg, 0.5f, Colours::aquamarine, snareImg, 0.5f, Colours::blueviolet);
    myhihatButton.setImages(false, true, true, hihatImg, 1.0f, Colours::transparentBlack, hihatImg, 0.5f, Colours::aquamarine, hihatImg, 0.5f, Colours::blueviolet);
    
    addAndMakeVisible(mykickButton);
    addAndMakeVisible(mysnareButton);
    addAndMakeVisible(myhihatButton);
    addAndMakeVisible(exportMidiButton);
    addAndMakeVisible(midiDrag);
    mykickButton.onClick = [this] { processor.hitkick = true;};
    mysnareButton.onClick = [this] { processor.hitsnare = true;};
    myhihatButton.onClick = [this] { processor.hithihat = true;};
    
    exportMidiButton.setClickingTogglesState(true);
    exportMidiButton.setColour(TextButton::textColourOnId, Colours::red);
    exportMidiButton.onClick = [this] { processor.recordMidi(exportMidiButton.getToggleState());};
    
    sliderAttach = new AudioProcessorValueTreeState::SliderAttachment (processor.treeState, "ONSET_THRESHOLD_ID", onset_threshold_slider);
    onset_threshold_slider.setSliderStyle(Slider::Slider::LinearBarVertical );
    onset_threshold_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::NoTextBox, true, 0, 0);
    onset_threshold_slider.setColour(Slider::trackColourId, Colours::grey.withAlpha(0.25f));
    onset_threshold_slider.setRange(-48.0f, 0.0f, 0.0f);
    onset_threshold_slider.setValue(-15.0f);

    addAndMakeVisible(processor.liveAudioScroller);
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
    auto area = getLocalBounds();
    processor.liveAudioScroller.setBounds (area.removeFromBottom(60));
    auto live_waveform_bounds = processor.liveAudioScroller.getBounds();
    onset_threshold_slider.setBounds(live_waveform_bounds);
    midiDrag.setBounds(area.removeFromTop(60));
    exportMidiButton.setBounds(area.removeFromTop(100));

    mykickButton.setBounds(area.getCentreX()-150,area.getCentreY()-20, 75,75);
    mysnareButton.setBounds(area.getCentreX()-20,area.getCentreY()-15, 50,50);
    myhihatButton.setBounds(area.getCentreX()+80,area.getCentreY()-30, 85,85);
    //    mykickButton.setBounds(100, 150, 50, 50);
//    mysnareButton.setBounds(200, 150, 50, 50);
//    myhihatButton.setBounds(300, 150, 50, 50);


}
