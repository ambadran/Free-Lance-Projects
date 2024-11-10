'''
terminal emulation
'''
from time import ticks_add, ticks_diff, ticks_ms, sleep

class Terminal:
    '''
    Terminal command executing abstraction
    '''
    def __init__(self, client, mic):
        self.client = client
        self.mic = mic

    def __call__(self, cmd):
        '''
        Process Commands
        '''
        if cmd.strip() == 'R':
            print("Sending audio sample")
            end_time = ticks_add(ticks_ms(), self.mic.sampling_duration)
            counter = 0
            while ticks_diff(end_time, ticks_ms()) > 0:
                self.client.send(self.mic.sample())
                counter += 1

            print(f"{counter} samples Sent!")

        else:
            print(f"Unknown command received: {cmd}")

 
