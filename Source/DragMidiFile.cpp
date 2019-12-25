//
//  DragMidiFile.cpp
//  DrumPad - VST3
//
//  Created by eugene upston on 7/14/19.
//

#include "DragMidiFile.hpp"

void DragMidiFile::mouseDrag(const MouseEvent& e)
{
    StringArray sArray;
    File temp_midi_file = File::getSpecialLocation( File::SpecialLocationType::tempDirectory).getChildFile( "deepbox.mid" );
    auto midi_path = temp_midi_file.getFullPathName();
    sArray.add(midi_path);
    DragAndDropContainer::performExternalDragDropOfFiles(sArray, true);
    imgBtnChangeOnDrag->setImages(false, true, true, *imgChangeOnDrag, 1.0f, Colours::white, *imgChangeOnDrag, 0.5f, Colours::white, *imgChangeOnDrag, 0.5f, Colours::white);
    
}
