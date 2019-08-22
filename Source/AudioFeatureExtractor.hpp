//
//  AudioFeatureExtractor.hpp
//  DrumPad
//
//  Created by eugene upston on 8/17/19.
//

#ifndef AudioFeatureExtractor_hpp
#define AudioFeatureExtractor_hpp
//test develop branch push
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
    
    AudioFeatureExtractor(int frame_size, int hop_size, int max_sample_length, int sample_rate=44100){
        essentia::init();
        AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
        
        fc = factory.create("FrameCutter", "frameSize", frame_size, "hopSize", hop_size, "startFromZero", true);
        win = factory.create("Windowing", "type", "hann");
        logNorm  = factory.create("UnaryOperator", "type", "log");
        spec  = factory.create("Spectrum");
        mfcc  = factory.create("MFCC");
        specContrast  = factory.create("SpectralContrast", "frameSize", frame_size + 1);
        energyband  = factory.create("EnergyBand", "startCutoffFrequency", 1000, "stopCutoffFrequency", 7000);
        energyband_low  = factory.create("EnergyBand", "startCutoffFrequency", 100, "stopCutoffFrequency", 700);
        
        const char* stats[] = { "mean", "var", "min", "max" };
        aggr = AlgorithmFactory::create("PoolAggregator",
                                        "defaultStats", arrayToVector<string>(stats));
        
        connect_buffer_to_algorithms();
    };
    
    void load_audio_buffer(AudioBuffer<float>& buffer){
        float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
        int size = buffer.getNumSamples();
        vector<Real> audio_buffer(start, start + size); // this will copy the data as a vector
        audiobuffer = audio_buffer;
//        audioBuffer.erase(audioBuffer.begin()+sampleSlice, audioBuffer.end()); //
    };
    
    
    void connect_buffer_to_algorithms(){
        fc->input("signal").set(audiobuffer); // initialize framecutter with signal

        fc->output("frame").set(frame);// set framecutter output with the frame
        win->input("frame").set(frame); //set windowed frame input to the frame

        win->output("frame").set(windowedFrame); // set windowframe output to frame
        spec->input("frame").set(windowedFrame); // set spectrum input to the windowed frame
        
        spec->output("spectrum").set(spectrum); // set output of spectrum to spectrum
        mfcc->input("spectrum").set(spectrum); // sets input of mfcc to spectrum
        
        mfcc->output("bands").set(mfccBands); // sets output of mfcc to mfccbands
        mfcc->output("mfcc").set(mfccCoeffs); // set output of mfcc to mfccoeffs
        
        logNorm->input("array").set(mfccBands);
        logNorm->output("array").set(mfccBandsLog);
        
        specContrast->input("spectrum").set(spectrum);
        specContrast->output("spectralContrast").set(spectralContrast);
        specContrast->output("spectralValley").set(spectralValley);
        
        energyband->input("spectrum").set(spectrum);
        energyband->output("energyBand").set(energy_freq_band);
        
        energyband_low->input("spectrum").set(spectrum);
        energyband_low->output("energyBand").set(energy_freq_band_low);
        
        aggr->input("input").set(pool);
        aggr->output("output").set(aggrPool);

    };
    
    void compute_algorithms(){
        while (true) {
            // compute a frame
            fc->compute();
            // if it was the last one (ie: it was empty), then we're done.
            if (!frame.size()) {
                break;
            }
            // if the frame is silent, just drop it and go on processing
            if (isSilent(frame)) continue;
            win->compute();
            spec->compute();
            mfcc->compute();
            specContrast->compute();
            energyband->compute();
            energyband_low->compute();
            logNorm->compute();

            pool.add("mfcc", mfccCoeffs);
            pool.add("spec", spectralContrast);
            pool.add("melbandlog", mfccBandsLog);
            pool.add("energy", energy_freq_band);
            pool.add("energylow", energy_freq_band_low);
        }
    };
    
    vector<float> compute_mean_features(){

        std::vector<float> audio_features;

        aggr->compute();
        vector<Real> mfcc_mean;
        vector<Real> spec_mean;
        vector<Real> melbandlog_mean;
        Real energy_mean;
        Real energylow_mean;
        
        mfcc_mean = aggrPool.value<vector<Real>>("mfcc.mean");
        spec_mean = aggrPool.value<vector<Real>>("spec.mean");
        melbandlog_mean = aggrPool.value<vector<Real>>("melbandlog.mean");
        energy_mean = aggrPool.value<Real>("energy.mean");
        energylow_mean = aggrPool.value<Real>("energylow.mean");
        
        for(auto mfcc : mfcc_mean){
            audio_features.push_back(mfcc);
        }
        for(auto spec : spec_mean){
            audio_features.push_back(spec);
        }
        for(auto melband : melbandlog_mean){
            audio_features.push_back(melband);
        }
        audio_features.push_back(energy_mean);
        audio_features.push_back(energylow_mean);

        return audio_features;
        
    };
    
    
    ~AudioFeatureExtractor(){
        delete fc;
        delete win;
        delete spec;
        delete mfcc;
        delete aggr;
        delete logNorm;
        delete specContrast;
        delete energyband_low;
        delete energyband;
        essentia::shutdown();
    };

private:
    int frame_size;
    int hop_size;
    int max_sample_length;
    int sample_rate;
    
    Pool pool;
    Pool aggrPool; // the pool with the aggregated MFCC values

    Algorithm* fc;
    Algorithm* win;
    Algorithm* logNorm;
    Algorithm* spec;
    Algorithm* mfcc;
    Algorithm* specContrast;
    Algorithm* energyband;
    Algorithm* energyband_low;
    Algorithm* aggr;
    
    vector<Real> spectrum, mfccCoeffs, mfccBands, mfccBandsLog;
    vector<Real> spectralContrast, spectralValley;
    vector<Real> frame, windowedFrame;
    Real energy_freq_band_low, energy_freq_band;
    
    vector<Real> audiobuffer;

};

#endif /* AudioFeatureExtractor_hpp */
