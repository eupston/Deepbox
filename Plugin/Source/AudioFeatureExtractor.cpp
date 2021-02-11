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
    eqloud     = factory.create("EqualLoudness");

    const char* stats[] = { "mean", "var", "min", "max" };
    aggr = AlgorithmFactory::create("PoolAggregator",
                                    "defaultStats", arrayToVector<string>(stats));
    
    connect_buffer_to_algorithms();
};


vector<Real> AudioFeatureExtractor::load_audio_buffer(std::vector<float> buffer)
{
    vector<Real> audio_buffer;
    for(auto sample : buffer){
        audio_buffer.push_back(sample);
    }
    audiobuffer = audio_buffer;
    return audiobuffer;
};


void AudioFeatureExtractor::connect_buffer_to_algorithms(){
    eqloud->input("signal").set(audiobuffer);
    eqloud->output("signal").set(audiobuffer);

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
    eqloud->compute();
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

        mfccPool.push_back(mfccCoeffs);
        specPool.push_back(spectralContrast);
        melbandlogPool.push_back(mfccBandsLog);
        energyhighPool.push_back(energy_freq_band_high);
        energylowPool.push_back(energy_freq_band_high);
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

    auto mean = [](vector<Real> const& v) {
        return std::accumulate(v.begin(), v.end(), 0LL) / v.size();
    };
    auto meanMatrix = [](vector<vector<Real>> const& vm) {
        vector<Real> vMean;
        for (int i = 0; i < vm.size(); i++){
            auto v = vm[i];
            vMean.push_back(std::accumulate(v.begin(), v.end(), 0LL) / v.size());
        };
        return vMean;
    };

    mfcc_mean = meanMatrix(mfccPool);
    spec_mean = meanMatrix(specPool);
    melbandlog_mean = meanMatrix(melbandlogPool);
    energyhigh_mean = mean(energyhighPool);
    energylow_mean = mean(energylowPool);

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
    delete eqloud;
    essentia::shutdown();
    

};
