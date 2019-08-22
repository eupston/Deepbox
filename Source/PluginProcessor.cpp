/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
DeepboxAudioProcessor::DeepboxAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    initialiseSynth();
}

DeepboxAudioProcessor::~DeepboxAudioProcessor()
{
}

//==============================================================================
const String DeepboxAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool DeepboxAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool DeepboxAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool DeepboxAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double DeepboxAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int DeepboxAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int DeepboxAudioProcessor::getCurrentProgram()
{
    return 0;
}

void DeepboxAudioProcessor::setCurrentProgram (int index)
{
}

const String DeepboxAudioProcessor::getProgramName (int index)
{
    return {};
}

void DeepboxAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void DeepboxAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    drumSynth.setCurrentPlaybackSampleRate(sampleRate);
}

void DeepboxAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool DeepboxAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void DeepboxAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    vector<float> audio_features;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();
    
//    float rmsLevel = buffer.getRMSLevel(0, 0, buffer.getNumSamples());
//    std::cout << "rmsLevel " << rmsLevel << std::endl;
//    
//    if(rmsLevel > 0.1){
        AudioFeatureExtractor my_audio_feature_exractor = AudioFeatureExtractor(512, 64, 1024, 44100);
//        my_audio_feature_exractor.load_audio_buffer(buffer);
//        my_audio_feature_exractor.compute_algorithms();
//        audio_features = my_audio_feature_exractor.compute_mean_features();
//        std::cout << "audio_features first: " << audio_features[0] << std::endl;
//        std::cout << "audio_features size: " << audio_features.size() << std::endl;
//        
//        int audio_feature_size = audio_features.size();
//        const fdeep::shared_float_vec audio_features_ptr(fplus::make_shared_ref<fdeep::float_vec>(audio_features));
//        fdeep::shape5 input_shape = fdeep::shape5(1, 1, 1, audio_feature_size, 1);
//        const auto result = mymodel.predict({fdeep::tensor5(input_shape, audio_features_ptr)});
//        std::vector<float> result_vec = *result.front().as_vector();
//        
//        int prediction_index = std::distance(result_vec.begin(), std::max_element(result_vec.begin(), result_vec.end()));
//        std::vector<std::string> drum_classes{"hihat","kick","snare"};
//        std::cout << drum_classes[prediction_index] << std::endl;
//        std::string drum_prediction = drum_classes[prediction_index];
//        if(drum_prediction == "hihat"){
//            hitkick = true;
//        }
//    }
    
    if(hitkick){
        triggerKickDrum(midiMessages);
        hitkick = false;
    }
    buffer.clear();
    drumSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    midiMessages.clear();

}

void DeepboxAudioProcessor::initialiseSynth()
{
    /** NOTE: - Quick and dirty sample drum synth for prototype
     *  In future versions will ideally allow user to select sample to use and
     *  also manage sample rate changes effect on loaded samples.
     */
    WavAudioFormat wavFormat;
    BigInteger kickNoteRange;
    BigInteger snareNoteRange;
    BigInteger hihatNoteRange;
    BigInteger osdTestSoundNoteRange;
    
    drumSynth.clearSounds();
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    
    std::unique_ptr<AudioFormatReader> readerKickDrum(formatManager.createReaderFor(File("/Volumes/Macintosh HD/Users/macuser/Desktop/MyCode/myjuce/DrumPad/Builds/MacOSX/Resources/bassdrum.wav")));
    kickNoteRange.setBit(kickNoteNumber);
    //    snareNoteRange.setBit(snareNoteNumber);
    //    hihatNoteRange.setBit(hihatNoteNumber);
    
    drumSynth.addSound(new SamplerSound("Kick Sound", *readerKickDrum, kickNoteRange, kickNoteNumber, 0.0, 0.0, 5.0));
    //    drumSynth.addSound(new SamplerSound("Snare Sound", *readerSnareDrum, snareNoteRange, snareNoteNumber, 0.0, 0.0, 5.0));
    //    drumSynth.addSound(new SamplerSound("HiHat Sound", *readerHiHat, hihatNoteRange, hihatNoteNumber, 0.0, 0.0, 5.0));
    
    drumSynth.addVoice(new SamplerVoice());
    
}

void DeepboxAudioProcessor::triggerKickDrum(MidiBuffer& midiMessages) const
{
    midiMessages.addEvent(MidiMessage::noteOn(1, kickNoteNumber, static_cast<uint8>(100)),0);
    
}


//==============================================================================
bool DeepboxAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* DeepboxAudioProcessor::createEditor()
{
    return new DeepboxAudioProcessorEditor (*this);
}

//==============================================================================
void DeepboxAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void DeepboxAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DeepboxAudioProcessor();
}
