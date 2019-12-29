//
//  AudioFeatureExtractor.cpp
//  DrumPad
//
//  Created by eugene upston on 8/17/19.
//

#include "AudioFeatureExtractor.hpp"

AudioFeatureExtractor::AudioFeatureExtractor(int frame_size, int hop_size, int sample_rate)
{
    essentia::init();
    AlgorithmFactory& factory = standard::AlgorithmFactory::instance();
    
    fc = factory.create("FrameCutter", "frameSize", frame_size, "hopSize", hop_size, "startFromZero", true);
    win = factory.create("Windowing", "type", "hann");
    logNorm  = factory.create("UnaryOperator", "type", "log");
    spec  = factory.create("Spectrum");
    mfcc  = factory.create("MFCC");
    specContrast  = factory.create("SpectralContrast", "frameSize", frame_size + 1);
    energyband_high  = factory.create("EnergyBand", "startCutoffFrequency", 1000, "stopCutoffFrequency", 8000);
    energyband_low  = factory.create("EnergyBand", "startCutoffFrequency", 100, "stopCutoffFrequency", 700);
    
    const char* stats[] = { "mean", "var", "min", "max" };
    aggr = AlgorithmFactory::create("PoolAggregator",
                                    "defaultStats", arrayToVector<string>(stats));
    
    connect_buffer_to_algorithms();
};


vector<Real> AudioFeatureExtractor::load_audio_buffer(vector<float> buffer)
{
    vector<Real> audio_buffer;
    int max_sample_size = 1024;
//    float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
//    int size = buffer.getNumSamples();
    int size = buffer.size();
    if (size < max_sample_size){
        // get an array of padded zeros
        int padded_zeros_size =  max_sample_size - size;
        vector<Real> padded_zero{static_cast<float>(padded_zeros_size)};
        
        //create audio_buffer vector<Real>
//        vector<Real> audio_buffer(start, start + max_sample_size); // this will copy the data as a vector
        for (int i=0; i<buffer.size(); i++){
            audio_buffer.push_back(buffer[i]);
        }
        
        // append array of padded zeros to audio_buffer
        audio_buffer.insert(audio_buffer.end(), padded_zero.begin(), padded_zero.end());
        audiobuffer = audio_buffer;
    }
    else{
//        vector<Real> audio_buffer(start, start + max_sample_size); // this will copy the data as a vector
        for (int i=0; i<buffer.size(); i++){
            audio_buffer.push_back(buffer[i]);
        }
        
        audiobuffer = audio_buffer;
    }
        
    return audiobuffer;
};


void AudioFeatureExtractor::connect_buffer_to_algorithms(){
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
    
    energyband_high->input("spectrum").set(spectrum);
    energyband_high->output("energyBand").set(energy_freq_band_high);
    
    energyband_low->input("spectrum").set(spectrum);
    energyband_low->output("energyBand").set(energy_freq_band_low);
    
    aggr->input("input").set(pool);
    aggr->output("output").set(aggrPool);
    
};

void AudioFeatureExtractor::compute_algorithms()
{
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
        energyband_high->compute();
        energyband_low->compute();
        logNorm->compute();
        
        pool.add("mfcc", mfccCoeffs);
        pool.add("spec", spectralContrast);
        pool.add("melbandlog", mfccBandsLog);
        pool.add("energyhigh", energy_freq_band_high);
        pool.add("energylow", energy_freq_band_low);
    }
};

vector<float> AudioFeatureExtractor::compute_mean_features()
{
    
    std::vector<float> audio_features;
    
    aggr->compute();
    vector<Real> mfcc_mean;
    vector<Real> spec_mean;
    vector<Real> melbandlog_mean;
    Real energyhigh_mean;
    Real energylow_mean;
    
    mfcc_mean = aggrPool.value<vector<Real>>("mfcc.mean");
    spec_mean = aggrPool.value<vector<Real>>("spec.mean");
    melbandlog_mean = aggrPool.value<vector<Real>>("melbandlog.mean");
    energyhigh_mean = aggrPool.value<Real>("energyhigh.mean");
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
    audio_features.push_back(energyhigh_mean);
    audio_features.push_back(energylow_mean);
    
//    for(auto feature : audio_features){
//        std::cout << feature << std::endl;
//    }
    return audio_features;
    
};

AudioFeatureExtractor::~AudioFeatureExtractor()
{
    delete fc;
    delete win;
    delete spec;
    delete mfcc;
    delete aggr;
    delete logNorm;
    delete specContrast;
    delete energyband_low;
    delete energyband_high;
    essentia::shutdown();
    

};
