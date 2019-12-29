from essentia.standard import *
import essentia.standard
import os
import fnmatch
from sklearn.model_selection import train_test_split
from keras.utils import to_categorical
import numpy as np
import librosa
import soundfile as sf

def create_audio_train_test_dataset_melbands(path_to_audio_files):
    """Converts all audio files in a given directory to mel spectrograms and splits into train and test"""
    # Get all directory with wavs
    audio_folder_names = [ f for f in os.listdir(path_to_audio_files) if f[0] != "."]
    largest_sample_size = get_largest_sample_size(path_to_audio_files)
    w = Windowing(type='hann')
    spectrum = Spectrum()
    mfcc = MFCC()
    logNorm = UnaryOperator(type='log')
    # get the mel band log for every audio file
    X = [] # all mel bands
    y = [] # all audio categories
    for audio_category in audio_folder_names:
        current_audio_folder = path_to_audio_files + "/" + audio_category
        for wav_file in recursively_find_files(current_audio_folder, '*.wav'):
            loader = essentia.standard.MonoLoader(filename=wav_file)
            audio = loader()
            if len(audio) < largest_sample_size:
                padded_zeros = np.zeros(largest_sample_size - len(audio))
                audio = np.concatenate((audio,padded_zeros),axis=None)
            audio = audio.astype('float32')
            mfccs = []
            melbands = []
            melbands_log = []
            for frame in FrameGenerator(audio, frameSize=512, hopSize=64, startFromZero=True):
                mfcc_bands, mfcc_coeffs = mfcc(spectrum(w(frame)))
                mfccs.append(mfcc_coeffs)
                melbands.append(mfcc_bands)
                melbands_log.append(logNorm(mfcc_bands))
            melbands_log = essentia.array(melbands_log).T
            X.append(melbands_log)
            y.append(index_of_classname(audio_category, audio_folder_names))
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=0.2, random_state=0)
    X_train = np.array(X_train,dtype=np.float32)
    X_test = np.array(X_test,dtype=np.float32)
    y_train = to_categorical(y_train, len(audio_folder_names))
    y_test = to_categorical(y_test, len(audio_folder_names))
    print(X_train.shape)
    return X_train, X_test, y_train, y_test

def create_audio_train_test_dataset_audio_features(path_to_audio_files, test_size=0.2):
    """extract audio features from all audio files in a given directory and splits into train and test"""
    # Get all directory with wavs
    audio_folder_names = [ f for f in os.listdir(path_to_audio_files) if f[0] != "."]
    print(audio_folder_names)
    # largest_sample_size = get_largest_sample_size(path_to_audio_files)
    largest_sample_size = 1024
    # get all audio features for every audio file
    X = [] # setup np empty array for all audio files
    y = [] # all audio categories
    for audio_category in audio_folder_names:
        current_audio_folder = path_to_audio_files + "/" + audio_category
        for wav_file in recursively_find_files(current_audio_folder, '*.wav'):
            audio_features = essentia_extract_feature(wav_file, pad_zeros=True, max_sample_size=largest_sample_size)
            X.append(audio_features)
            y.append(index_of_classname(audio_category, audio_folder_names))
    X = np.array(X, dtype=np.float32)
    X = np.expand_dims(X, axis=2)

    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_size, random_state=0)
    y_train = to_categorical(y_train, len(audio_folder_names))
    y_test = to_categorical(y_test, len(audio_folder_names))

    return X_train, X_test, y_train, y_test

def get_largest_sample_size(path_to_audio_files):
    """Recursively searches folder and find the largest sample size"""
    largest_sample_size = 0
    for wav_file in recursively_find_files(path_to_audio_files, '*.wav'):
        loader = essentia.standard.MonoLoader(filename=wav_file)
        audio = loader()
        if len(audio) > largest_sample_size:
            largest_sample_size = len(audio)
    return largest_sample_size

def recursively_find_files(directory, pattern):
    """recursively searches a directory for specific file type and yields results"""
    for root, dirs, files in os.walk(directory):
        for basename in files:
            if fnmatch.fnmatch(basename, pattern):
                filename = os.path.join(root, basename)
                yield filename

def index_of_classname(class_name, class_names):
    """Returns the index of the class name from all class name"""
    idx = class_names.index(class_name)
    return idx

def onset_detect(audio_vector, sr):
    """Returns the onsets detected of a given audio file"""
    ####### Onset and Tempo detection ##########
    onset = librosa.onset.onset_detect(y=audio_vector, sr=sr, backtrack=True)
    onset_env = librosa.onset.onset_strength(y=audio_vector, sr=sr)
    onset_sec = librosa.frames_to_time(onset, sr=sr)
    onset_frames = librosa.frames_to_samples(onset)
    return onset_frames

def extract_feature(file_name):
    """Generates feature input (mfccs, chroma, mel, contrast, tonnetz).
    -*- author: mtobeiyf https://github.com/mtobeiyf/audio-classification -*-
    """
    X, sample_rate = sf.read(file_name, dtype='float32')
    if X.ndim > 1:
        X = X[:,0]
    X = X.T
    stft = np.abs(librosa.stft(X, n_fft=2048))
    mfccs = np.mean(librosa.feature.mfcc(y=X, sr=sample_rate, n_mfcc=40).T,axis=0)
    chroma = np.mean(librosa.feature.chroma_stft(S=stft, sr=sample_rate).T,axis=0)
    mel = np.mean(librosa.feature.melspectrogram(X, sr=sample_rate).T,axis=0)
    contrast = np.mean(librosa.feature.spectral_contrast(S=stft, sr=sample_rate).T,axis=0)
    tonnetz = np.mean(librosa.feature.tonnetz(y=librosa.effects.harmonic(X), sr=sample_rate).T,axis=0)
    return mfccs, chroma, mel, contrast, tonnetz


def essentia_extract_feature(path_to_audio_file, pad_zeros=False, max_sample_size=0):
    """Generates feature input (mfccs, energyband, melband_log, spectral contrast)."""
    w = Windowing(type='hann')
    spectrum = Spectrum()
    mfcc = MFCC()
    logNorm = UnaryOperator(type='log')
    framesize = 512
    specContrast = SpectralContrast(frameSize=int(framesize+1))
    energybandhigh = EnergyBand(startCutoffFrequency=1000, stopCutoffFrequency=8000)
    energybandlow = EnergyBand(startCutoffFrequency=100, stopCutoffFrequency=700)
    loader = essentia.standard.EqloudLoader(filename=path_to_audio_file,replayGain=-2)
    audio = loader()
    if pad_zeros:
        if len(audio) < max_sample_size:
            padded_zeros = np.zeros(max_sample_size - len(audio))
            audio = np.concatenate((audio, padded_zeros), axis=None)
        elif len(audio) > max_sample_size:
            audio = audio[:max_sample_size]
        audio = audio.astype('float32')
    mfccs = []
    melbands_log = []
    spec_contrast = []
    energy_band_high = []
    energy_band_low = []
    for frame in FrameGenerator(audio, frameSize=framesize, hopSize=64, startFromZero=True):
        mfcc_bands, mfcc_coeffs = mfcc(spectrum(w(frame)))
        spectralContrast, spectralValley = specContrast(spectrum(w(frame)))
        energy_freq_high_band = energybandhigh(spectrum(w(frame)))
        energy_freq_low_band = energybandlow(spectrum(w(frame)))
        melbands_log.append(logNorm(mfcc_bands))
        mfccs.append(mfcc_coeffs)
        spec_contrast.append(spectralContrast)
        energy_band_high.append(energy_freq_high_band)
        energy_band_low.append(energy_freq_low_band)
    mfccs = essentia.array(mfccs)
    spec_contrast = essentia.array(spec_contrast)
    melbands_log = essentia.array(melbands_log)
    energy_band_high = essentia.array(energy_band_high)
    energy_band_low = essentia.array(energy_band_low)

    mfccs = np.mean(mfccs, axis=0)
    spec_contrast = np.mean(spec_contrast, axis=0)
    melbands_log = np.mean(melbands_log, axis=0)
    energy_band_high = np.mean(energy_band_high, axis=0)
    energy_band_low = np.mean(energy_band_low, axis=0)
    ext_features = np.hstack([mfccs, spec_contrast, melbands_log,energy_band_low,energy_band_high])
    return ext_features

if __name__=="__main__":
    # X_train, X_test, y_train, y_test = create_audio_train_test_dataset_audio_features("/Volumes/Macintosh HD/Users/macuser/Desktop/MyCode/mypython/testEssentia/testWavs")
    audio_features = essentia_extract_feature("/Volumes/Macintosh HD/Users/macuser/Desktop/Datasets/myBeatBoxDataset/mytestset/snare/snare.wav",pad_zeros=True, max_sample_size=5000)
