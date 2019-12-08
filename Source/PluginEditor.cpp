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
    
//    auto kickImg = ImageCache::getFromMemory(BinaryData::kick_drum_icon_png, BinaryData::kick_drum_icon_pngSize);
//    auto snareImg = ImageCache::getFromMemory(BinaryData::snare_icon_png, BinaryData::snare_icon_pngSize);
//    auto hihatImg = ImageCache::getFromMemory(BinaryData::hihat_icon_png, BinaryData::snare_icon_pngSize);
//
//    
    auto kickImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/kick_drum_vec.png").getFullPathName());
    auto snareImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/snare_drum_vec.png").getFullPathName());
    auto hihatImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/hihat_vec.png").getFullPathName());

    
    auto deepbox_lips_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/deepbox_lips.png").getFullPathName());
    
    midiDrag.setImages(false, true, true, deepbox_lips_img, 1.0f, Colours::transparentWhite, deepbox_lips_img, 0.5f, Colours::aquamarine, deepbox_lips_img, 0.5f, Colours::blueviolet);
    
    
    mykickButton.setImages(false, true, true, kickImg, 1.0f, Colours::transparentBlack, kickImg, 0.5f, Colours::aquamarine, kickImg, 0.5f, Colours::blueviolet);
    mysnareButton.setImages(false, true, true, snareImg, 1.0f, Colours::transparentBlack, snareImg, 0.5f, Colours::aquamarine, snareImg, 0.5f, Colours::blueviolet);
    myhihatButton.setImages(false, true, true, hihatImg, 1.0f, Colours::transparentBlack, hihatImg, 0.5f, Colours::aquamarine, hihatImg, 0.5f, Colours::blueviolet);
    
    addAndMakeVisible(mykickButton);
    addAndMakeVisible(mysnareButton);
    addAndMakeVisible(myhihatButton);
    addAndMakeVisible(recordMidiButton);
    addAndMakeVisible(midiDrag);
    mykickButton.onClick = [this] { processor.hitkick = true;};
    mysnareButton.onClick = [this] { processor.hitsnare = true;};
    myhihatButton.onClick = [this] { processor.hithihat = true;};
    
    recordMidiButton.setClickingTogglesState(true);
    recordMidiButton.setColour(TextButton::textColourOnId, Colours::red);
    recordMidiButton.onClick = [this] { processor.recordMidi(recordMidiButton.getToggleState());};
    
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
    recordMidiButton.setBounds(area.removeFromTop(20));
    midiDrag.setBounds(area.getCentreX() -120,area.getCentreY() -180,250,250);
//    midiDrag.setBounds(area.removeFromTop(60));

    mykickButton.setBounds(area.getCentreX() -140,area.getCentreY() +30, 75,75);
    mysnareButton.setBounds(area.getCentreX() -30,area.getCentreY() +30, 75,75);
    myhihatButton.setBounds(area.getCentreX() +80,area.getCentreY() +30, 75,75);

}
