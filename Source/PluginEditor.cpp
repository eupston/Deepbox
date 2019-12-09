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
    setSize (400, 400);
    
//    auto kickImg = ImageCache::getFromMemory(BinaryData::kick_drum_icon_png, BinaryData::kick_drum_icon_pngSize);
//    auto snareImg = ImageCache::getFromMemory(BinaryData::snare_icon_png, BinaryData::snare_icon_pngSize);
//    auto hihatImg = ImageCache::getFromMemory(BinaryData::hihat_icon_png, BinaryData::snare_icon_pngSize);

    
    auto kickImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/kick_drum_vec.png").getFullPathName());
    auto snareImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/snare_drum_vec.png").getFullPathName());
    auto hihatImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/hihat_vec.png").getFullPathName());

    
    auto deepbox_lips_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/deepbox_lips_inverse.png").getFullPathName());
    auto record_button_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/record_button.png").getFullPathName());

    midiDrag.setImages(false, true, true, deepbox_lips_img, 1.0f, Colours::white, deepbox_lips_img, 0.5f, Colours::white, deepbox_lips_img, 0.5f, Colours::blueviolet);
    recordButton.setImages(false, true, true, record_button_img, 1.0f, Colour(242,8,123), record_button_img, 0.5f, Colours::mediumvioletred, record_button_img, 0.5f, Colours::blueviolet);
    
    mykickButton.setImages(false, true, true, kickImg, 1.0f, Colours::white, kickImg, 0.5f, Colours::mediumvioletred, kickImg, 0.5f, Colours::violet);
    mysnareButton.setImages(false, true, true, snareImg, 1.0f, Colours::white, snareImg, 0.5f, Colours::mediumvioletred, snareImg, 0.5f, Colours::violet);
    myhihatButton.setImages(false, true, true, hihatImg, 1.0f, Colours::white, hihatImg, 0.5f, Colours::mediumvioletred, hihatImg, 0.5f, Colours::violet);
    
    addAndMakeVisible(mykickButton);
    addAndMakeVisible(mysnareButton);
    addAndMakeVisible(myhihatButton);
//    addAndMakeVisible(recordMidiButton);
    addAndMakeVisible(midiDrag);
    addAndMakeVisible(recordButton);
    
    mykickButton.onClick = [this] { processor.hitkick = true;};
    mysnareButton.onClick = [this] { processor.hitsnare = true;};
    myhihatButton.onClick = [this] { processor.hithihat = true;};
    
    recordButton.setClickingTogglesState(true);
//    recordMidiButton.setColour(TextButton::textColourOnId, Colours::red);
    recordButton.onClick = [this] { processor.recordMidi(recordButton.getToggleState());};
    
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
//    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));
    g.fillAll(Colour(51,11,63));
    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void DeepboxAudioProcessorEditor::resized()
{
    int drum_width = 50;
    int drum_height = 50;
    int drum_spacer = 130;
    int lips_width = 400;
    int lips_height = 250;
    int lips_y_spacer = 10;
    int drums_y_axis_add = 100;
    int record_width = 40;
    int record_height = 40;
    int record_y_spacer = -1;
    
    
    auto area = getLocalBounds();
    processor.liveAudioScroller.setBounds (area.removeFromBottom(60));
    auto live_waveform_bounds = processor.liveAudioScroller.getBounds();
    onset_threshold_slider.setBounds(live_waveform_bounds);
//    recordMidiButton.setBounds(area.removeFromTop(20));
    
    midiDrag.setBounds(area.getCentreX() - lips_width/2, lips_y_spacer, lips_width, lips_height);
    recordButton.setBounds(area.getCentreX() - record_width/2, (lips_height/2) + lips_y_spacer + record_y_spacer, record_width, record_height);
    

    mykickButton.setBounds(area.getCentreX() - drum_spacer, area.getCentreY() + drums_y_axis_add, drum_width, drum_height);
    mysnareButton.setBounds(area.getCentreX() - (drum_width/2), area.getCentreY() + drums_y_axis_add, drum_width, drum_height);
    myhihatButton.setBounds(area.getCentreX() + drum_spacer - drum_width, area.getCentreY() + drums_y_axis_add, drum_width, drum_height);

}
