'''
This Script will:
    - send command to receive tcp/ip adc values directly from the Controller
    - run it through the AI model to determine if it's a distress call
    - send result back to MCU
'''
import socket
import wave
from time import sleep
import struct
import wave

class Station:
    '''
    Abstraction to the Station that controls the Pico W
    '''
    controller_IP = '192.168.1.7'
    controller_PORT = 1234
    SAMPLING_RATE = 8000
    SAMPLE_DURATION = 6
    START_INDICATOR_STRING = b"Start\n\n"
    END_INDICATOR_STRING = b"\n\nEnd\n"

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(0.5)

    def send(self, cmd):
        '''
        sends commands to Pico
        '''
        self.sock.send(cmd)

    def recv_audio(self):
        '''
        receives data
        '''
        try:
            data = bytearray()
            while True:
                data.extend(self.sock.recv(4096))
                if b'End' in data:
                    print(f"Received: {len(data)} ")
                    return data

        except TimeoutError:
            raise TimeoutError("No response from MicroPython Device!")

    def strip_data(self, data) -> bytearray:
        '''
        The MicroPython Device should put 
            'Start\n\n' and a '\n\nEnd\n'
        at the start and end of the audio sample

        return the stripped bytearray

        by checking the presence of start and end indicators
        this makes sure I started listenning and kept listenning 
        from begging till end
        '''
        # finding the index of 'Start\n\n' in the data
        # should be at zero
        start_ind = data.find(self.START_INDICATOR_STRING)
        if start_ind == -1:
            raise ValueError("Couldn't find Start index")

        # finding the index of '\n\nEnd\n' in the data
        # should be at len(data)
        end_ind = data.find(self.END_INDICATOR_STRING)
        if end_ind== -1:
            raise ValueError("Couldn't find End index")

        data = data[ start_ind+len(self.START_INDICATOR_STRING) : end_ind]

        return data
        
    def to_integers(self, data: bytearray) -> list[int]:
        '''
        converts the bytearray object which holds 2-byte integers next to each other to list of integers
        '''
        return list(struct.unpack(f"<{len(data)//2}H", data))

    def export_wav_file(self, samples, filename):
        '''
        exports list of 12-bit ADC integer values into a .wav file
        '''
        with wave.open(filename, 'w') as wav_file:
            # Set the parameters for the WAV file
            wav_file.setnchannels(1)  # Mono audio
            wav_file.setsampwidth(2)  # 2 bytes per sample (16-bit audio)
            wav_file.setframerate(14000)  # Set the sample rate


            max_val = max(samples)
            scaled_data = [(sample * 32767) // max_val for sample in samples]

            # Convert the integer list to a byte stream and write to the file
            wav_data = struct.pack(f'<{len(samples)}H', *samples)
            wav_file.writeframes(wav_data)

        print(f"Wrote {filename} with {len(samples)}B")

    def get_audio_sample(self, duration=1000):
        '''
        :duration: duration of audio sample in ms
        get audio sample by sending command and then receiving
        '''
        # Send get Audio Command with specific duration
        try:
            self.send(f"R{duration}\n".encode())

        except BrokenPipeError:

            self.sock.connect((self.controller_IP, self.controller_PORT))
            print("Connection to Pico W Successful!")

            self.send(f"R{duration}\n".encode())

        # Wait until audio is captured
        sleep(duration/1000)

        # Start receiving the data
        data = self.recv_audio()

        # Stripping the data (checking start-end valididity too)
        data = self.strip_data(data)

        # Checking length of data
        #TODO: should send from Pico len(samples)

        # Converting to list of integers 
        data = self.to_integers(data)

        # Exporting to .wave file
        self.export_wav_file(data, 'audio.wav')

        return data



station = Station()
if __name__ == '__main__':
    station.get_audio_sample()
            



