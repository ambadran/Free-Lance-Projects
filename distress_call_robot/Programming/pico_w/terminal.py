'''
terminal emulation
'''
from time import ticks_add, ticks_diff, ticks_ms, sleep
from micropython import mem_info
import machine
import gc
import re
from distress_finder import DistressCallFinder

class Terminal:
    '''
    Terminal command executing abstraction
    '''
    def __init__(self, server, mic1, mic2, mic3, differential_drive, 
            gps):
        self.server = server
        self.mic1 = mic1
        self.mic2 = mic2
        self.mic3 = mic3
        self.mics = (self.mic1, self.mic2, self.mic3)
        self.differential_drive = differential_drive
        self.gps = gps
        self.distress_call_finder = DistressCallFinder(self.mic1,
                                                       self.mic2,
                                                       self.mic3)

    def print(self, string):
        '''
        prints text both in micropython terminal and in tcp connection
        '''
        print(string)
        self.server.client.sendall(f"{string}\n")

    def __call__(self, cmd):
        '''
        Process Commands
        Possible Commands
        g -> get GPS
        [fbrl]<int> -> forward, backward, right, left
        R<mic index>D<duration> -> record from specific mic a specific duration
        F -> Distress Call find orientation
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
        command_patterns = re.compile(r'([GFRfbrlg])(\d*)(D\d+)?')
        matched = command_patterns.match(cmd)
        if matched:
            matched = matched.groups()

            ### Step 2: check for logical erros
            if matched[0] in ['f', 'b', 'r', 'l']:
                if not matched[1]:
                    self.print("Can't send a movement command without specifying how much!")
                    return None

                elif (int(matched[1])) == 0:
                    self.print("Can't move 0 anything!")
                    return None

            if matched[0] in ['G', 'F', 'g']:
                if matched[1] or matched[2]:
                    self.print("Command doesn't support parameters")
                    return None

            if matched[0] == 'R':
                if not matched[1]:
                    self.print("Must specific which mic to record from")
                    return None

                elif not matched[1].isdigit():
                    self.print("Mic Index must be an integer")
                    return None

                # Get mic_index
                mic_index = int(matched[1])
                if mic_index < 0 or mic_index > 2:
                    self.print("Mic Index is from [0, 1, 2]")
                    return None

                # Get Record duration
                if not matched[2]:
                    self.print("Must specific record duration in ms")
                    return None

                elif not matched[2][1:].isdigit():
                    self.print("Record duration must be an integer")
                    return None

                self.mics[mic_index].sampling_duration = int(matched[2][1:])
                if self.mics[mic_index].sampling_duration < 0 or self.mics[mic_index].sampling_duration > self.mics[mic_index].max_record_length():
                    self.print(f"Record Duration must be  0ms < duration < {self.mics[mic_index].max_record_length()}ms")
                    return None

            ### Step 3: Execution
            if matched[0] == 'R':
                # Get samples
                samples = self.mics[mic_index].get_samples()

                # Sending samples
                self.server.send_audio(samples)

                gc.collect()
                self.print(f"Free Memory: {gc.mem_free()}\n")

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

            elif matched[0] == 'g':
                self.print(self.gps.raw_statements)

            elif matched[0] == 'F':
                self.print(f"Orientation: {self.distress_call_finder.find_orientation()}")

            elif matched[0] == 'G':
                # Step1: Find source then orient towards 
                self.differential_drive.orient(self.distress_call_finder.find_orientation())

                # Step2 : Move Forward while listening for maximum
                # Value. ALso correct orientation along the way
                #TODO:
                self.differential_drive.forward(300)


        elif cmd == '':
            # empty string is what telnet and nc commands send when they close 
            # their session for some reason, therefore, will reset device
            # when detected
            self.print("Received empty string, could be session close \n\nRestarting Machine..\n")
            machine.reset()

        else:
            # Didn't match the regex!
            self.print(f"Unknown command received: {cmd}")

 
