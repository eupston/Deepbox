//
//  AudioFeatureExtractor.hpp
//  DrumPad
//
//  Created by eugene upston on 8/17/19.
//

#ifndef AudioFeatureExtractor_hpp
#define AudioFeatureExtractor_hpp

#include <stdio.h>
#include <vector>
#include "essentia/algorithmfactory.h"
#include "essentia/essentiamath.h"
#include "essentia/pool.h"
#include "../JuceLibraryCode/JuceHeader.h"

//develop merge

using namespace essentia;
using namespace essentia::standard;
using namespace std;

class AudioFeatureExtractor{
    
public:
    
    AudioFeatureExtractor(int frame_size, int hop_size, int max_sample_length, int sample_rate=44100);
    
    vector<Real> load_audio_buffer(AudioBuffer<float>& buffer);
    
    void connect_buffer_to_algorithms();
    
    void compute_algorithms();
    
    vector<float> compute_mean_features();
    
    ~AudioFeatureExtractor();

private:
    int frame_size;
    int hop_size;
    int max_sample_length;
    int sample_rate;
    
    Pool pool;
    Pool aggrPool;
    
    Algorithm* fc;
    Algorithm* win;
    Algorithm* logNorm;
    Algorithm* spec;
    Algorithm* mfcc;
    Algorithm* specContrast;
    Algorithm* energyband;
    Algorithm* energyband_low;
    Algorithm* aggr;
    
    vector<Real> audiobuffer;
    vector<Real> spectrum, mfccCoeffs, mfccBands, mfccBandsLog;
    vector<Real> spectralContrast, spectralValley;
    vector<Real> frame, windowedFrame;
    Real energy_freq_band_low, energy_freq_band;
        
    

};

#endif /* AudioFeatureExtractor_hpp */
