'''
terminal emulation
'''
from time import ticks_add, ticks_diff, ticks_ms, sleep
from micropython import mem_info
import gc

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
        if cmd.isspace():
            print('ok..')
            return None

        cmd = cmd.strip()
        if cmd.startswith('R'):

            if len(cmd) != 1:
                try:
                    duration = int(cmd[1:])
                    self.mic.sampling_duration = duration
                except ValueError:
                    print(f"Couldn't convert command 'R' Parameter into an integer, given command: {cmd}")
                    return None


            gc.collect()
            samples = bytearray(self.mic.sampling_rate*int(self.mic.sampling_duration/1000)*2)
            print(f"Capturing {int(len(samples)/2)} samples in {self.mic.sampling_duration} ms audio..\n")

            s_time = ticks_ms()
            for ind in range(0, len(samples), 2):
                # samples[ind:ind+2] = self.mic.adc.read_u16().to_bytes(2, 'H')
                value = self.mic.adc.read_u16()
                samples[i * 2] = value & 0xFF  # Lower byte
                samples[i * 2 + 1] = (value >> 8) & 0xFF  # Upper byte
            e_time = ticks_diff(ticks_ms(), s_time)

            print(f"Read samples in {e_time}ms!")
            print(f"Sample Rate: {round((int(len(samples)/2))/(e_time/1000))}\n\n")


            print("Sending audio sample..")
            s_time = ticks_ms()
            self.client.send(samples)
            e_time = ticks_diff(ticks_ms(), s_time)
            print(f"Sent samples in {e_time}ms!\n")

            gc.collect()
            print(f"Free Memory: {gc.mem_free()}")
            print()

        else:
            print(f"Unknown command received: {cmd}")

 
