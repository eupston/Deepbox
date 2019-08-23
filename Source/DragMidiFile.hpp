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
    
    DragMidiFile(){};
    void paint (Graphics& g) override;
    void mouseDrag(const MouseEvent& e) override;
    
private:
    
};

#endif /* DragMidiFile_hpp */

