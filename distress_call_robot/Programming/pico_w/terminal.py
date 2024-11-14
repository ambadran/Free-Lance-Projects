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
    def __init__(self, server, mic):
        self.server = server
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

            # Get samples
            samples = self.mic.get_samples()

            # Sending samples
            self.server.send_audio(samples)

            gc.collect()
            print(f"Free Memory: {gc.mem_free()}")
            print()

        else:
            print(f"Unknown command received: {cmd}")

 
