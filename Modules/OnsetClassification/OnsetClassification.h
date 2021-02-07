/*
  ==============================================================================

    OnsetClassification.h
    Created: 16 Nov 2013 5:44:30pm
    Author:  Govinda Ram Pingali

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <vector>
#include <numeric>
#include <algorithm>
#include <stdio.h>
#include <iostream>


#include "Source/ShortTermFourierTransform.h"
#include "Source/AudioFeatureExtraction.h"



#define MAX_DECAY_WINDOW_BLOCKS     25          // For Block Size of 512 ~ 250 ms
#define MIN_DECAY_WINDOW_BLOCKS     2           // For Block Size of 512 ~ 
#define MAX_DELTA_THRESHOLD         4.0F
#define MIN_DELTA_THRESHOLD         0.6F
#define MAX_BUFFER_SIZE             2048


//========================================================================================================================

using std::vector;
using namespace juce;

class OnsetClassification
{
   

public:
    
    
    //==============================================================================
    // Constructor and Destructor
    
    OnsetClassification();
    ~OnsetClassification();

    
    //==============================================================================
    // Audio Device Settings Structure
    
    struct AudioDeviceSettings
    {
        int         iBufferSize;
        int         iNumBins;
    };
    
    
    
    //==============================================================================
    // Detection Parameters Structure
    
    struct DetectionParameters
    {
        //--- Sensitivity Settings ---//
        double  dDeltaThreshold;
        int     iDecayBlockWindow;
    };
    
    
    
    //==============================================================================
    // Onset Detection Variables Structure
    
    struct DetectionVariables
    {
        //--- Moving Average for Threshold ---//
        double      dAdaptiveThreshold;
        bool        bDecayPeriod;
        int         iDecayBlockCounter;
    };
    
    
    
    //==============================================================================
    // Classification Parameters Structure
    
    struct ClassificationParameters
    {
        int iNumClasses;
        int iNumFeatures;
        int iNumObservations;
        double dCrossValidationAccuracy;
        bool bDidFinishTraining;
        int iCurrentObservation;
    };
    
    
    
    //==============================================================================
    // Audio Features Structure
    
    struct AudioFeatures
    {
        double   dSpectralCentroid;
        
        double   dSpectralFlux;
        double   dRootMeanSquare_dB;
    };
    
    
    
    //==============================================================================
    // Initialize - Set Buffer Sizes
    
    void initialize(int blockSize, double sampleRate);
    
    
    
    //==============================================================================
    // Process Methods
    
    bool detectOnset(const float** audioBuffer);

private:

    //==============================================================================
    // Parameters
    
    AudioDeviceSettings         m_sDeviceSettings;
    DetectionParameters         m_sDetectionParameters;
    ClassificationParameters    m_sTrainingParameters;
    DetectionVariables          m_sDetectionVariables;
    AudioFeatures               m_sAudioFeatures;

    //==============================================================================
    // Class Declarations

    ScopedPointer<ShortTermFourierTransform>        m_pcDetectionSTFT;
    ScopedPointer<AudioFeatureExtraction>           m_pcAudioFeature;

    //==============================================================================
    // Buffers
    
    //--- Audio ---//
    vector<float>           m_pfDetectionAudioBlock;
    vector<float>           m_pfClassificationAudioBlock;
    
    //--- FFT ---//
    vector<float>           m_pfDetectionCurrentRealFFT;
    vector<float>           m_pfDetectionCurrentImgFFT;
    vector<float>           m_pfDetectionPreviousRealFFT;
    vector<float>           m_pfClassificationRealFFT;
    vector<float>           m_pfClassificationImgFFT;
    
    //--- Classification ---//
    vector<double>           m_pdFeatureVector;
    vector<double>           m_pdNormalizedVector;
    vector<double>           m_pdFeatureMaxVector;
    vector<double>           m_pdFeatureMinVector;
    
    vector<vector<double>>   m_ppdTrainingData;
    vector<vector<double>>   m_ppdNormalizedData;
    vector<int>              m_piTrainingClassLabels;
    
    vector<double>           m_pdProbabilityEstimates;
    
    
    //--- Evaluation and ReTraining ---//
    vector<vector<double>>   m_ppdEvaluationData;
    vector<int>              m_piEvaluationLabels;
    
    String                   m_psFeatureVector;

    StringArray              m_psAudioFilePaths;

};

//========================================================================================================================

