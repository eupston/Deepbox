import keras
from keras.models import Sequential
from keras.layers import Conv2D, Convolution2D, Conv1D, GlobalAveragePooling1D, Activation
from keras.layers import MaxPooling2D, MaxPooling1D
from keras.layers import Flatten
from keras.layers import Dense
from keras.layers import Dropout

def audio_CNN_1D_model(input_shape, num_classes):
    model = Sequential()
    model.add(Conv1D(32, kernel_size=3, activation='relu', input_shape=input_shape))
    model.add(Conv1D(64, kernel_size=3, activation='relu'))
    model.add(MaxPooling1D(pool_size=2))
    model.add(Dropout(0.25))
    model.add(Flatten())
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(num_classes, activation='softmax'))
    model.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.adam(), metrics=['accuracy'])
    return model

def audio_CNN_1D_model_2(input_shape, num_classes):
    model = Sequential()
    model.add(Conv1D(64, 3, input_shape=input_shape))
    model.add(Activation('relu'))
    model.add(Conv1D(64, 3))
    model.add(Activation('relu'))
    model.add(MaxPooling1D(3))
    model.add(Conv1D(128, 3))
    model.add(Activation('relu'))
    model.add(Conv1D(128, 3))
    model.add(Activation('relu'))
    model.add(GlobalAveragePooling1D())
    model.add(Dropout(0.5))
    model.add(Dense(num_classes))
    model.add(Activation("softmax"))
    model.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.adam(), metrics=['accuracy'])
    return model

def audio_CNN_2D_model(input_shape, num_classes):
    model = Sequential()
    model.add(Conv2D(32, kernel_size=(3, 3), activation='relu', input_shape=input_shape))
    model.add(Conv2D(64, kernel_size=(3, 3), activation='relu'))
    model.add(MaxPooling2D(pool_size=(2, 2)))
    model.add(Dropout(0.25))
    model.add(Flatten())
    model.add(Dense(128, activation='relu'))
    model.add(Dropout(0.5))
    model.add(Dense(num_classes, activation='softmax'))
    model.compile(loss=keras.losses.categorical_crossentropy, optimizer=keras.optimizers.adam(), metrics=['accuracy'])
    return model