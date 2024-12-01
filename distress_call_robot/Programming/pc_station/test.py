import numpy as np
import librosa
import pyaudio
import pickle

# Constants
CHUNK = 1024  # Number of audio samples per chunk
FORMAT = pyaudio.paInt16  # Audio format (16-bit)
CHANNELS = 1  # Mono audio
RATE = 22050  # Sampling rate (samples per second)

def testing_unit(audio_data, sr):
    tester = []
    mfccs = np.mean(librosa.feature.mfcc(y=audio_data, sr=sr, n_mfcc=40).T, axis=0)
    tester.append(mfccs)
    tester = np.array(tester)
    return tester  # Return MFCCs extracted array

def svm_process(audio_data, sr):
    load_model = pickle.load(open('phase1_model.sav', 'rb'))  # Loading phase_1 model (noise vs speech)
    # result_decision = load_model.decision_function(testing_unit(audio_data, sr))
    result = load_model.predict(testing_unit(audio_data, sr))  # Predicting if result[0]==1 then noise else human sound

    # print(f"Phase-1 Decision Values: {result_decision}")

    if result[0] == 2:  # Checking if sound is noise or human
        print("Phase-1 clear")
        load_model2 = pickle.load(open('phase2_model.sav', 'rb'))  # Loading phase2 model
        ok = load_model2.predict(testing_unit(audio_data, sr))  # Using second phase_model

        if ok[0] == 1:
            print('Scream')
            return True
        else:
            print('Speech')
            return False
    else:
        print("Noise")
        return "Noise"

def real_time_processing():
    p = pyaudio.PyAudio()

    stream = p.open(format=FORMAT,
                    channels=CHANNELS,
                    rate=RATE,
                    input=True,
                    frames_per_buffer=CHUNK)

    print("Listening...")

    try:
        while True:
            data = stream.read(CHUNK)
            audio_data = np.frombuffer(data, dtype=np.int16)
            audio_data = audio_data.astype(np.float32) / 32768.0  # Normalize to [-1, 1]

            result = svm_process(audio_data, RATE)
            print(result)

    except KeyboardInterrupt:
        print("Stopping...")

    finally:
        stream.stop_stream()
        stream.close()
        p.terminate()

def analyze_wav_file():
    p = pyaudio.PyAudio()
    #TODO:
    

if __name__ == "__main__":
    real_time_processing()
