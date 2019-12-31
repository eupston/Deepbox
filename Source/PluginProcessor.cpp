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
    essentia::init();

    NormalisableRange<float> onset_threshold_range(-48.0f, 0.0f);
    mykickButton.onClick = [this] { hitkick = true;};
    mysnareButton.onClick = [this] { hitsnare = true;};
    myhihatButton.onClick = [this] { hithihat = true;};

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
    sample_Rate = sampleRate;
    samples_Per_Block = samplesPerBlock;
    
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
    my_onset_detector.initialize(samples_Per_Block, sample_Rate);
    vector<float> audio_features;
    float mag = buffer.getMagnitude(0, 0, buffer.getNumSamples());
    float db = Decibels::gainToDecibels(mag);
    float current_onset_threshold = onset_threshold_slider.getValue();
    auto currentValuesInBuffer = buffer.getArrayOfReadPointers();
    bool onset_detected = my_onset_detector.detectOnset(currentValuesInBuffer);
    if(!onset_below_floor_threshold && db < floor_onset_threshold){
        onset_below_floor_threshold = true;
    }
    
    if(onset_detected && db > current_onset_threshold && onset_below_floor_threshold){
        onset_below_floor_threshold = false;
        std::vector<float> audioBuffer = addPaddedZeros(buffer, maxSampleSize);
        AudioFeatureExtractor my_audio_feature_exractor = AudioFeatureExtractor(512, 64, sample_Rate);
        my_audio_feature_exractor.load_audio_buffer(audioBuffer);
        my_audio_feature_exractor.compute_algorithms();
        audio_features = my_audio_feature_exractor.compute_mean_features();
        int audio_feature_size = audio_features.size();
        const fdeep::shared_float_vec audio_features_ptr(fplus::make_shared_ref<fdeep::float_vec>(audio_features));
        fdeep::shape5 input_shape = fdeep::shape5(1, 1, 1, audio_feature_size, 1);
        const auto result = mymodel.predict({fdeep::tensor5(input_shape, audio_features_ptr)});
        std::vector<float> result_vec = *result.front().as_vector();
        int prediction_index = std::distance(result_vec.begin(), std::max_element(result_vec.begin(), result_vec.end()));
        std::string drum_prediction = drum_classes[prediction_index];
        std::cout << "prediction result: " + fdeep::show_tensor5s(result) << std::endl;
        std::cout << "drum_prediction: " << drum_prediction << std::endl;

        if(drum_prediction == "kick"){
            mykickButton.triggerClick();
        }

        if(drum_prediction == "snare"){
            mysnareButton.triggerClick();
        }

        if(drum_prediction == "hihat"){
            myhihatButton.triggerClick();
        }
        
    }
    // update tempo and ms per ticks
    playHead = this->getPlayHead();
    playHead->getCurrentPosition(currentPositionInfo);
    tempo = currentPositionInfo.bpm;
    msPerTick = (60000.f / tempo) / 960.f; //960 ticks per quarternote

    if(hitkick){
        auto midi_on_off = triggerKickDrum(midiMessages, msPerTick);
        mms.addEvent(midi_on_off[0]);
        mms.addEvent(midi_on_off[1]);
        hitkick = false;
    }

    if(hitsnare){
        auto midi_on_off = triggerSnareDrum(midiMessages, msPerTick);
        mms.addEvent(midi_on_off[0]);
        mms.addEvent(midi_on_off[1]);
        hitsnare = false;
    }

    if(hithihat){
        auto midi_on_off = triggerHihatDrum(midiMessages, msPerTick);
        mms.addEvent(midi_on_off[0]);
        mms.addEvent(midi_on_off[1]);
        hithihat = false;
    }
    
    liveAudioScroller.pushBuffer(buffer);
    
    buffer.clear();
    drumSynth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
    midiMessages.clear();

}

void DeepboxAudioProcessor::initialiseSynth()
{
    
    WavAudioFormat wavFormat;
    BigInteger kickNoteRange;
    BigInteger snareNoteRange;
    BigInteger hihatNoteRange;
    BigInteger osdTestSoundNoteRange;
    
    drumSynth.clearSounds();
    
    AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<AudioFormatReader> readerKickDrum(formatManager.createReaderFor(File::getSpecialLocation( File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/bassdrum.wav")));
    std::unique_ptr<AudioFormatReader> readerSnareDrum(formatManager.createReaderFor(File::getSpecialLocation( File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/snaredrum.wav")));
    std::unique_ptr<AudioFormatReader> readerHiHat(formatManager.createReaderFor(File::getSpecialLocation( File::SpecialLocationType::currentApplicationFile).getChildFile("Contents/Resources/hihat.wav")));

    kickNoteRange.setBit(kickNoteNumber);
    snareNoteRange.setBit(snareNoteNumber);
    hihatNoteRange.setBit(hihatNoteNumber);
    
    drumSynth.addSound(new SamplerSound("Kick Sound", *readerKickDrum, kickNoteRange, kickNoteNumber, 0.0, 0.0, 5.0));
    drumSynth.addSound(new SamplerSound("Snare Sound", *readerSnareDrum, snareNoteRange, snareNoteNumber, 0.0, 0.0, 5.0));
    drumSynth.addSound(new SamplerSound("HiHat Sound", *readerHiHat, hihatNoteRange, hihatNoteNumber, 0.0, 0.0, 5.0));
    drumSynth.addVoice(new SamplerVoice());
}

std::vector<float> DeepboxAudioProcessor::addPaddedZeros(AudioBuffer<float> buffer, int maxSampleSize){
    
    float* start = buffer.getWritePointer(0); // get the pointer to the first sample of the first channel
    int size = buffer.getNumSamples();
    if (size < maxSampleSize){
        // get an array of padded zeros
        int padded_zeros_size =  maxSampleSize - size;
        std::vector<float> audio_buffer_vec(start, start + size); // this will copy the data as a vector
        // append array of padded zeros to audio_buffer
        for(int i=0; i < padded_zeros_size; i++){
            audio_buffer_vec.push_back(0.0f);
        }
        return audio_buffer_vec;

    }
    else{
        std::vector<float> audio_buffer_vec(start, start + maxSampleSize);
        return audio_buffer_vec;
    }

}


vector<MidiMessage> DeepboxAudioProcessor::triggerKickDrum(MidiBuffer& midiMessages, double msPerTick) const
{
    MidiMessage midikickdrumon = MidiMessage::noteOn(1, kickNoteNumber, static_cast<uint8>(100));
    MidiMessage midikickdrumoff = MidiMessage::noteOff(1, kickNoteNumber);
    midiMessages.addEvent(midikickdrumon,0);
    double timeStampInMS = Time::getMillisecondCounterHiRes() - startTime;
    midikickdrumon.setTimeStamp(timeStampInMS / msPerTick);
    midikickdrumoff.setTimeStamp(timeStampInMS / msPerTick + 20);
    vector<MidiMessage> miditoadd = {midikickdrumon,midikickdrumoff};
    return miditoadd;
    
}

vector<MidiMessage> DeepboxAudioProcessor::triggerSnareDrum(MidiBuffer& midiMessages, double msPerTick) const
{
    MidiMessage midisnaredrumon = MidiMessage::noteOn(1, snareNoteNumber, static_cast<uint8>(100));
    MidiMessage midisnaredrumoff = MidiMessage::noteOff(1, snareNoteNumber);
    midiMessages.addEvent(midisnaredrumon,0);
    double timeStampInMS = Time::getMillisecondCounterHiRes() - startTime;
    midisnaredrumon.setTimeStamp(timeStampInMS / msPerTick);
    midisnaredrumoff.setTimeStamp(timeStampInMS / msPerTick + 20);
    vector<MidiMessage> miditoadd = {midisnaredrumon,midisnaredrumoff};
    return miditoadd;
    
}

vector<MidiMessage> DeepboxAudioProcessor::triggerHihatDrum(MidiBuffer& midiMessages, double msPerTick) const
{
    MidiMessage midihihatdrumon = MidiMessage::noteOn(1, hihatNoteNumber, static_cast<uint8>(100));
    MidiMessage midihihatdrumoff = MidiMessage::noteOff(1, hihatNoteNumber);
    midiMessages.addEvent(midihihatdrumon,0);
    double timeStampInMS = Time::getMillisecondCounterHiRes() - startTime;
    midihihatdrumon.setTimeStamp(timeStampInMS / msPerTick);
    midihihatdrumoff.setTimeStamp(timeStampInMS / msPerTick + 20);
    vector<MidiMessage> miditoadd = {midihihatdrumon,midihihatdrumoff};
    return miditoadd;

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


void DeepboxAudioProcessor::recordMidi(bool isRecording)
{
    if (isRecording){
        mms.clear();
        startTime = Time::getMillisecondCounterHiRes();
        microsecondsPerQuarter = (60000.f / tempo) * 1000.f;
        MidiMessage tempoEvent = MidiMessage::tempoMetaEvent(microsecondsPerQuarter);
        tempoEvent.setTimeStamp(0);
        mms.addEvent(tempoEvent);
        liveAudioScroller.setColours(Colours::black, Colour(242,8,123));
        deepbox_text.setImages(false, true, true, deepbox_text_img, 1.0f, Colours::white, deepbox_text_img, 0.5f, Colours::white, deepbox_text_img, 0.5f, Colours::white);
    }
    
    else{
        MidiFile midiFile;
        midiFile.setTicksPerQuarterNote(960);
        midiFile.addTrack(mms);
        File outputFile = File::getSpecialLocation( File::SpecialLocationType::tempDirectory).getChildFile( "deepbox.mid" );
        outputFile.deleteFile();
        FileOutputStream outputStream(outputFile);
        midiFile.writeTo(outputStream);
        outputStream.flush();
        mms.clear();
        liveAudioScroller.setColours(Colours::black, Colours::white);
        deepbox_text.setImages(false, true, true, draglips_text_img, 1.0f, Colours::white, draglips_text_img, 0.5f, Colours::white, draglips_text_img, 0.5f, Colours::white);

    }

}

void DeepboxAudioProcessor::debugBufferWavFile(std::vector<float> audioBuffer){
    AudioBuffer<float> tempbuffer(1, maxSampleSize);
    float* start = tempbuffer.getWritePointer(0);
    int size = tempbuffer.getNumSamples();
    
    for(int sample = 0; sample < size; ++sample ){
        start[sample] = audioBuffer[sample];
    }
    
    File file = File::getSpecialLocation( File::SpecialLocationType::userDesktopDirectory).getChildFile( "debug_deepbox.wav" );
    
    WavAudioFormat format;
    std::unique_ptr<AudioFormatWriter> writer;
    writer.reset (format.createWriterFor (new FileOutputStream (file),
                                          sample_Rate,
                                          1,
                                          24,
                                          {},
                                          0));
    if (writer != nullptr)
        writer->writeFromAudioSampleBuffer (tempbuffer, 0, tempbuffer.getNumSamples());
    
}




//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new DeepboxAudioProcessor();
}
