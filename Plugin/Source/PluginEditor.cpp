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
    auto kickImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/kick_drum_vec.png").getFullPathName());
    auto snareImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/snare_drum_vec.png").getFullPathName());
    auto hihatImg = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/hihat_vec.png").getFullPathName());
    auto deepbox_lips_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/deepbox_lips_inverse.png").getFullPathName());
    auto record_button_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/record_button.png").getFullPathName());
    auto stop_button_img = ImageCache::getFromFile(File::getSpecialLocation(File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/imgs/stop_button.png").getFullPathName());
    midiDrag.setImages(false, true, true, deepbox_lips_img, 1.0f, Colours::white, deepbox_lips_img, 0.5f, Colours::white, deepbox_lips_img, 0.5f, Colour(242,8,123));
    recordButton.setImages(false, true, true, record_button_img, 1.0f, Colour(242,8,123), record_button_img, 0.5f, Colours::mediumvioletred, stop_button_img, 0.5f, Colour(242,8,123));
    processor.deepbox_text.setImages(false, true, true, processor.deepbox_text_img, 1.0f, Colours::white, processor.deepbox_text_img, 0.5f, Colours::white, processor.deepbox_text_img, 0.5f, Colours::white);
    processor.mykickButton.setImages(false, true, true, kickImg, 1.0f, Colours::white, kickImg, 0.5f, Colours::mediumvioletred, kickImg, 0.5f, Colours::violet);
    processor.mysnareButton.setImages(false, true, true, snareImg, 1.0f, Colours::white, snareImg, 0.5f, Colours::mediumvioletred, snareImg, 0.5f, Colours::violet);
    processor.myhihatButton.setImages(false, true, true, hihatImg, 1.0f, Colours::white, hihatImg, 0.5f, Colours::mediumvioletred, hihatImg, 0.5f, Colours::violet);

    midiDrag.imgBtnChangeOnDrag = &processor.deepbox_text;
    midiDrag.imgChangeOnDrag = &processor.deepbox_text_img;
    
    recordButton.setClickingTogglesState(true);
    recordButton.onClick = [this] { processor.recordMidi(recordButton.getToggleState());};
    
    processor.onset_threshold_slider.setSliderStyle(Slider::Slider::LinearBarVertical);
    processor.onset_threshold_slider.setTextBoxStyle(Slider::TextEntryBoxPosition::TextBoxBelow, true, 0, 0);
    processor.onset_threshold_slider.setColour(Slider::trackColourId, Colours::grey.withAlpha(0.25f));
    processor.onset_threshold_slider.setRange(-48.0f, 0.0f, 1);
    processor.onset_threshold_slider.setValue(-15.0f);
    processor.onset_threshold_slider.setTextValueSuffix(" db");

    addAndMakeVisible(midiDrag);
    addAndMakeVisible(processor.deepbox_text);
    addAndMakeVisible(processor.mykickButton);
    addAndMakeVisible(processor.mysnareButton);
    addAndMakeVisible(processor.myhihatButton);
    addAndMakeVisible(processor.liveAudioScroller);
    addAndMakeVisible(&processor.onset_threshold_slider);
    addAndMakeVisible(recordButton);

}

DeepboxAudioProcessorEditor::~DeepboxAudioProcessorEditor()
{
}

//==============================================================================
void DeepboxAudioProcessorEditor::paint (Graphics& g)
{
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
    int drums_y_shift = 100;
    int record_width = 40;
    int record_height = 40;
    int record_y_shift = -1;
    int deepbox_text_height = 40;
    int deepbox_text_width = 170;
    int deepbox_text_x_shift = 5;
    int deepbox_text_y_shift = 5;
    
    auto area = getLocalBounds();
    processor.liveAudioScroller.setBounds (area.removeFromBottom(60));
    auto live_waveform_bounds = processor.liveAudioScroller.getBounds();
    processor.onset_threshold_slider.setBounds(live_waveform_bounds);
    
    midiDrag.setBounds(area.getCentreX() - lips_width/2, lips_y_spacer, lips_width, lips_height);
    recordButton.setBounds(area.getCentreX() - record_width/2, (lips_height/2) + lips_y_spacer + record_y_shift, record_width, record_height);
    processor.deepbox_text.setBounds(area.getCentreX() - deepbox_text_width/2 - deepbox_text_x_shift, (lips_height/2) + deepbox_text_y_shift, deepbox_text_width, deepbox_text_height);
    
    processor.mykickButton.setBounds(area.getCentreX() - drum_spacer, area.getCentreY() + drums_y_shift, drum_width, drum_height);
    processor.mysnareButton.setBounds(area.getCentreX() - (drum_width/2), area.getCentreY() + drums_y_shift, drum_width, drum_height);
    processor.myhihatButton.setBounds(area.getCentreX() + drum_spacer - drum_width, area.getCentreY() + drums_y_shift, drum_width, drum_height);

}
