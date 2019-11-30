//
//  DragMidiFile.cpp
//  DrumPad - VST3
//
//  Created by eugene upston on 7/14/19.
//

#include "DragMidiFile.hpp"


void DragMidiFile::paint (Graphics& g)
{
    g.fillAll (Colours::black.withAlpha (0.5f));
    g.setColour (Colours::white);
    g.drawRect (getLocalBounds(), 3);
    g.drawFittedText("Drag Midi File", getLocalBounds(), Justification::centred, 2);
}


void DragMidiFile::mouseDrag(const MouseEvent& e)
{
    StringArray sArray;
    File temp_midi_file = File::getSpecialLocation( File::SpecialLocationType::tempDirectory).getChildFile( "deepbox.mid" );
    auto midi_path = temp_midi_file.getFullPathName();
    sArray.add(midi_path);
    DragAndDropContainer::performExternalDragDropOfFiles(sArray, true);
}
