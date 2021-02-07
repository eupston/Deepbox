/*
  ==============================================================================

    OnsetClassification.cpp
    Created: 16 Nov 2013 5:44:30pm
    Author:  Govinda Ram Pingali

  ==============================================================================
*/

#include "OnsetClassification.h"
#include "Source/AudioFeatureExtraction.cpp"
#include "Source/ShortTermFourierTransform.cpp"
#include "Source/AudioFFT.cpp"

OnsetClassification::OnsetClassification()
{

    //--- Initializing Device Settings ---//
    m_sDeviceSettings.iBufferSize                   =   0;
    m_sDeviceSettings.iNumBins                      =   0;

    
    
    
    //--- Initializing Detection Parameters ---//
    m_sDetectionParameters.iDecayBlockWindow    =   MIN_DECAY_WINDOW_BLOCKS;
    m_sDetectionParameters.dDeltaThreshold      =   MIN_DELTA_THRESHOLD;
    
    
    
    //--- Initializing Training Parameters ---//
    m_sTrainingParameters.iNumFeatures          =   3;
    m_sTrainingParameters.iNumClasses           =   0;
    m_sTrainingParameters.iNumObservations      =   0;
    m_sTrainingParameters.dCrossValidationAccuracy = 0.0f;
    m_sTrainingParameters.bDidFinishTraining    =   false;
    m_sTrainingParameters.iCurrentObservation   =   0;
    
    
    
    //--- Initializing Buffers ---//
    m_pfDetectionCurrentImgFFT.assign(0, 0.0f);
    m_pfDetectionCurrentRealFFT.assign(0, 0.0f);
    m_pfDetectionPreviousRealFFT.assign(0, 0.0f);
    m_pfClassificationRealFFT.assign(0, 0.0f);
    m_pfClassificationImgFFT.assign(0, 0.0f);
    m_pfClassificationAudioBlock.assign(0, 0.0f);
    m_pfDetectionAudioBlock.assign(0, 0.0f);
    
    //--- Initialize Classification Vectors ---//
    m_pdFeatureVector.assign(m_sTrainingParameters.iNumFeatures, 0.0f);
    m_pdFeatureMaxVector.assign(m_sTrainingParameters.iNumFeatures, 0.0f);
    m_pdFeatureMinVector.assign(m_sTrainingParameters.iNumFeatures, 0.0f);
    m_pdNormalizedVector.assign(0, 0.0f);
    m_piTrainingClassLabels.assign(0, 0);
    m_ppdTrainingData.assign(0, vector<double> (0,0.0f));
    m_ppdNormalizedData.assign(0, vector<double> (0,0.0f));
    m_pdProbabilityEstimates.assign(1, 0.0f);
    m_ppdEvaluationData.assign(0, vector<double> (0,0.0f));
    m_piEvaluationLabels.assign(0, 0);

    m_psFeatureVector.clear();
    
    
    //--- Allocating Memory / Creating Instances for Utility Objects ---//
    m_pcAudioFeature            = new AudioFeatureExtraction();
    m_pcDetectionSTFT           = nullptr;

    
    //--- Initializing Variables ---//
    m_sDetectionVariables.dAdaptiveThreshold    =   0.0f;
    m_sDetectionVariables.iDecayBlockCounter    =   0;
    m_sDetectionVariables.bDecayPeriod          =   false;
    
    m_sAudioFeatures.dSpectralCentroid          =   0.0f;
    m_sAudioFeatures.dSpectralFlux              =   0.0f;
    m_sAudioFeatures.dRootMeanSquare_dB         =   0.0f;
    
    for (int i=0; i<16; i++)
    {
        m_psAudioFilePaths.add("");
    }
    
}




//==============================================================================
// Set Audio Device Settings - Initializer Method (Called in Prepare To Play)
//==============================================================================

void OnsetClassification::initialize(int blockSize, double sampleRate)
{
    m_sDeviceSettings.iBufferSize          = blockSize;
    m_sDeviceSettings.iNumBins             = (m_sDeviceSettings.iBufferSize / 2) + 1;
    
    
    //--- Resize Buffers based on new blockSize ---//
    m_pfDetectionAudioBlock.assign(m_sDeviceSettings.iBufferSize, 0.0f);
    m_pfClassificationAudioBlock.assign((2 * m_sDeviceSettings.iBufferSize), 0.0f);
    
    m_pfDetectionCurrentRealFFT.assign(m_sDeviceSettings.iNumBins, 0.0f);
    m_pfDetectionCurrentImgFFT.assign(m_sDeviceSettings.iNumBins, 0.0f);
    m_pfDetectionPreviousRealFFT.assign(m_sDeviceSettings.iNumBins, 0.0f);
    
    m_pfClassificationRealFFT.assign(blockSize + 1, 0.0f);
    m_pfClassificationImgFFT.assign(blockSize + 1, 0.0f);
    
    
    //--- Re-initalize STFT based on block size ---//
    m_pcDetectionSTFT       =   nullptr;
    m_pcDetectionSTFT       =   new ShortTermFourierTransform(blockSize);
}




//==============================================================================
// Destructor
//==============================================================================
OnsetClassification::~OnsetClassification()
{
    m_pfDetectionCurrentRealFFT.clear();
    m_pfDetectionPreviousRealFFT.clear();
    m_pfDetectionCurrentImgFFT.clear();
    
    m_pfClassificationRealFFT.clear();
    m_pfClassificationImgFFT.clear();
    
    m_pfClassificationAudioBlock.clear();
    
    m_pdFeatureVector.clear();
    m_pdFeatureMaxVector.clear();
    m_pdFeatureMinVector.clear();
    m_pdNormalizedVector.clear();
    m_ppdTrainingData.clear();
    m_piTrainingClassLabels.clear();
    m_ppdNormalizedData.clear();
    m_pdProbabilityEstimates.clear();
    m_ppdEvaluationData.clear();
    m_piEvaluationLabels.clear();
    
    m_psFeatureVector.clear();
    
    m_psAudioFilePaths.clear();
    
    m_pcDetectionSTFT       = nullptr;
    m_pcAudioFeature        = nullptr;
}






//====================================== Process Methods ============================================


//==============================================================================
// Return True if Audio Block Consists of an Onset
// !!! Running on Audio Thread
//==============================================================================
bool OnsetClassification::detectOnset(const float** audioBuffer)
{
    m_pfDetectionAudioBlock.insert(m_pfDetectionAudioBlock.begin(), audioBuffer[0], audioBuffer[0] + m_sDeviceSettings.iBufferSize);
//
    

    //--- Compute STFT on 1st channel ---//
    m_pcDetectionSTFT->computeFFT(m_pfDetectionAudioBlock.data(),
                                  m_pfDetectionCurrentRealFFT.data(),
                                  m_pfDetectionCurrentImgFFT.data());



    //--- Compute Spectral Flux ---//
    m_sAudioFeatures.dSpectralFlux = m_pcAudioFeature->spectralFlux(m_pfDetectionPreviousRealFFT.data(),
                                                                    m_pfDetectionCurrentRealFFT.data(),
                                                                    m_sDeviceSettings.iBufferSize);



    //--- Create Adaptive Threshold ---//
    m_sDetectionVariables.dAdaptiveThreshold =  m_sDetectionParameters.dDeltaThreshold
                                                +   ((m_sAudioFeatures.dSpectralFlux + m_sDetectionVariables.dAdaptiveThreshold) / 2.0f);



    //--- Store Current FFT ---//
    m_pfDetectionPreviousRealFFT = m_pfDetectionCurrentRealFFT;

    
    
    //--- Store Current Audio Buffer ---//
    for (int i=0; i < m_sDeviceSettings.iBufferSize; i++)
    {
        m_pfClassificationAudioBlock[i] = m_pfDetectionAudioBlock[i];
    }
    
    
    //--- Compute RMS ---//
    m_sAudioFeatures.dRootMeanSquare_dB = m_pcAudioFeature->rootMeanSquareIndB(audioBuffer, m_sDeviceSettings.iBufferSize);
    
    
    //--- Check for Peaks ---//
    if (m_sAudioFeatures.dSpectralFlux > m_sDetectionVariables.dAdaptiveThreshold)
    {
        if (m_sDetectionVariables.iDecayBlockCounter == 0)
        {
            m_sDetectionVariables.bDecayPeriod = true;
            m_sDetectionVariables.iDecayBlockCounter++;
            m_sTrainingParameters.iCurrentObservation++;
            
            return true;
        }
    }
    
    
    
    //--- If Not Decay, Reset Counter ---//
    if (m_sDetectionVariables.bDecayPeriod)
    {
        if (m_sDetectionVariables.iDecayBlockCounter >= m_sDetectionParameters.iDecayBlockWindow)
        {
            m_sDetectionVariables.bDecayPeriod = false;
            m_sDetectionVariables.iDecayBlockCounter = 0;
        }
        
        else
        {
            m_sDetectionVariables.iDecayBlockCounter++;
        }
    }
    
    return false;
    
}