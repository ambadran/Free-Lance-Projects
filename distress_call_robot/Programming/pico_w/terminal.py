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

            print(f"Capturing {self.mic.sampling_duration} sec audio..")

            # dividing the audio sample into 100ms sections
            samples = []
            end_time = ticks_add(ticks_ms(), self.mic.sampling_duration)
            while ticks_diff(end_time, ticks_ms()) > 0:
                samples.append(self.mic.adc.read_u16())

            print("Done!\n\nSending audio sample..")
            s_time = ticks_ms()
            for sample in samples:
                self.client.send(f"{sample}\n")
            e_time = ticks_diff(ticks_ms(), s_time)

            print(f"{len(samples)} samples Sent in {e_time}ms!\n")

        else:
            print(f"Unknown command received: {cmd}")

 
