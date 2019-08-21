//
//  DragMidiFile.hpp
//  DrumPad - VST3
//
//  Created by eugene upston on 7/14/19.
//

#ifndef DragMidiFile_hpp
#define DragMidiFile_hpp

#include "../JuceLibraryCode/JuceHeader.h"

class DragMidiFile  :   public Component,
                        public DragAndDropContainer

{
public:
    
    DragMidiFile(){}
    void paint (Graphics& g) override
    {
        g.fillAll (Colours::black.withAlpha (0.5f));
        g.setColour (Colours::white);
        g.drawRect (getLocalBounds(), 3);
        g.drawFittedText("Drag Midi File", getLocalBounds(), Justification::centred, 2);
    }

    void mouseDrag(const MouseEvent& e) override{
        StringArray sArray;
        sArray.add("/Volumes/Macintosh HD/Users/macuser/Desktop/MyCode/myjuce/DrumPad/Source/Resources/beatbox.mid");
        DragAndDropContainer::performExternalDragDropOfFiles(sArray, true);
    }
    
private:
    
};

#endif /* DragMidiFile_hpp */

