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

using namespace essentia;
using namespace essentia::standard;
using namespace std;

class AudioFeatureExtractor{
    
public:
    
    AudioFeatureExtractor(int frame_size, int hop_size, int sample_rate=44100);
    
    vector<Real> load_audio_buffer(std::vector<float> buffer);
    void connect_buffer_to_algorithms();
    void compute_algorithms();
    vector<float> compute_mean_features();
    
    ~AudioFeatureExtractor();

private:

    Pool pool;
    Pool aggrPool;
    
    Algorithm* fc;
    Algorithm* win;
    Algorithm* logNorm;
    Algorithm* spec;
    Algorithm* mfcc;
    Algorithm* specContrast;
    Algorithm* energyband_high;
    Algorithm* energyband_low;
    Algorithm* aggr;
    Algorithm* eqloud;
    
    vector<Real> audiobuffer;
    vector<Real> spectrum, mfccCoeffs, mfccBands, mfccBandsLog;
    vector<Real> spectralContrast, spectralValley;
    vector<Real> frame, windowedFrame;
    Real energy_freq_band_low, energy_freq_band_high;

};

#endif /* AudioFeatureExtractor_hpp */
