'''
terminal emulation
'''
from time import ticks_add, ticks_diff, ticks_ms, sleep
from micropython import mem_info
import machine
import gc
import re

class Terminal:
    '''
    Terminal command executing abstraction
    '''
    def __init__(self, server, mic, differential_drive):
        self.server = server
        self.mic = mic
        self.differential_drive = differential_drive

    def print(self, string):
        '''
        prints text both in micropython terminal and in tcp connection
        '''
        print(string)
        self.server.client.sendall(f"{string}\n\n")

    def __call__(self, cmd):
        '''
        Process Commands
        '''
        # Receiving Command
        try:
            cmd = cmd.decode()

        except UnicodeError:
            self.print("Unicode Error: User sent wrong characters")
            return None

        if cmd.isspace():
            self.print('ok..')  # testing connection
            return None

        ### Step 1:  identifying command
        cmd = cmd.strip()
        command_patterns = re.compile(r'([Rfbrl])(\d*)')
        matched = command_patterns.match(cmd)
        if matched:
            matched = matched.groups()

            ### Step 2: check for logical erros
            if (matched[0] in ['f', 'b', 'r', 'l']) and not matched[1]:
                self.print("Can't send a movement command without specifying how much!")
                return None

            if (int(matched[1])) == 0:
                self.print("Can't record or move 0 anything!")
                return None

            ### Step 3: Execution
            if matched[0] == 'R':
                if matched[1]:
                    self.mic.sampling_duration = int(matched[1])

                # Get samples
                samples = self.mic.get_samples()

                # Sending samples
                self.server.send_audio(samples)

                gc.collect()
                print(f"Free Memory: {gc.mem_free()}\n")

            elif matched[0] == 'f':
                self.differential_drive.forward(int(matched[1]))
                self.print(f"Moving Forward for {int(matched[1])}ms")

            elif matched[0] == 'b':
                self.differential_drive.backward(int(matched[1]))
                self.print(f"Moving Backward for {int(matched[1])}ms")

            elif matched[0] == 'r':
                self.differential_drive.right(int(matched[1]))
                self.print(f"Moving Right: {int(matched[1])}deg")

            elif matched[0] == 'l':
                self.differential_drive.left(int(matched[1]))
                self.print(f"Moving Left: {int(matched[1])}deg")

        elif cmd == '':
            # empty string is what telnet and nc commands send when they close 
            # their session for some reason, therefore, will reset device
            # when detected
            self.print("Received empty string, could be session close \n\nRestarting Machine..\n")
            machine.reset()

        else:
            print(f"Unknown command received: {cmd}")

 
