import keras
from models import *
from utils import *
from keras.models import load_model
import fdeepConvertModel

#------------ Global variables ---------------

dataset_path = "/Volumes/Macintosh HD/Users/macuser/Desktop/Datasets/myBeatBoxDataset/Train"
test_dataset_path = "/Volumes/Macintosh HD/Users/macuser/Desktop/Datasets/myBeatBoxDataset/Test"
model_path = 'models/my_deepbox_model_v2.h5'
fdeep_model_path = 'models/my_deepbox_model_v2.json'

#------------ Train ---------------
def train_model(dataset_path):
    X_train, X_test, y_train, y_test = create_audio_train_test_dataset_audio_features(dataset_path)
    model = audio_CNN_1D_model((X_train.shape[1],X_train.shape[2]), len(y_train[0]))
    print(model.summary())
    model.fit(X_train, y_train, batch_size=64, epochs=200, verbose=1, validation_data=(X_test, y_test))
    return model

#---------- Save model ------------
def save_keras_model(model, model_path):
    model.save(model_path)

# -------- load model -------------
def load_keras_model(model_path):
    model = load_model(model_path)
    return model

# ---------- Single Prediction -----------------
def single_prediction(model):
    test_data = []
    x_data = essentia_extract_feature("/Volumes/Macintosh HD/Users/macuser/Desktop/Datasets/myBeatBoxDataset/Test/deepbox_debug.wav", pad_zeros=True, max_sample_size=1024)
    test_data.append(x_data)
    test_data = np.expand_dims(test_data, axis=2)
    for item in test_data[0]:
        print(str(item[0]) + ",")
    print('done')
    pred = model.predict(test_data)
    classes = ['hihat', 'kick', 'snare']
    print(pred)
    print("Single Prediction", classes[np.argmax(pred)])

#---- evaluate Unseen Test set ---------
def evaluate_test_set(model, test_dataset_path):
    X_train, X_test, y_train, y_test = create_audio_train_test_dataset_audio_features(test_dataset_path, test_size=0.01)
    score = model.evaluate(x=X_train, y=y_train, batch_size=64, verbose=1)
    print("Model Loss", score[0])
    print("Model Accuracy", score[1])


if __name__ == "__main__":
    # model = train_model(dataset_path)
    # save_keras_model(model, model_path)
    # model = load_keras_model(model_path)
    # evaluate_test_set(model, test_dataset_path)
    # #
    model = load_keras_model(model_path)
    single_prediction(model)

    #convert to fdeep json model
    # fdeepConvertModel.convert(model_path, fdeep_model_path)