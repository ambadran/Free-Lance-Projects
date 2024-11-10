'''
This Script will:
    - send command to receive tcp/ip adc values directly from the Controller
    - run it through the AI model to determine if it's a distress call
    - send result back to MCU
'''
import socket
import wave
import time

class Station:
    '''
    Abstraction to the Station that controls the Pico W
    '''
    controller_IP = '192.168.1.8'
    controller_PORT = 1234
    SAMPLING_RATE = 8000
    SAMPLE_DURATION = 6

    def __init__(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.connect((self.controller_IP, self.controller_PORT))
        self.sock.settimeout(1)
        print("Connection to Pico W Successful!")

    def send(self, cmd):
        '''
        sends commands to Pico
        '''
        self.sock.send(cmd)

    def recv(self, size=1024) -> str | None:
        '''
        receives data
        '''
        try:
            data = self.sock.recv(size)  # Each ADC reading is 2 bytes
            return data.decode()
        except TimeoutError:
            return None

    def get_audio_sample(self):
        '''
        get audio sample by sending command and then receiving
        '''
        values = []
        self.send(b"R\n")
        num_samples = self.SAMPLING_RATE*self.SAMPLE_DURATION
        while True:
            latest = self.recv()
            if latest is None:
                break
            latest = latest.strip().split('\n')
            print(f"Received: {len(values)}, latest: {latest[-1]} ", end='\r')
            values.extend(latest)


        # Received Values !
        print(f"Recieved {len(values)} samples!!")
        self.values = values

        # for ind, value in enumerate(self.values):
        #     self.values[ind] = int(value)

        # print(self.values)

station = Station()
if __name__ == '__main__':
    station.get_audio_sample()
            



