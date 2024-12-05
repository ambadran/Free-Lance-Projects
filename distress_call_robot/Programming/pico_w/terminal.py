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
            gps, ultrasonic_sensor1, ultrasonic_sensor2, PIR_sensor1, PIR_sensor2):
        self.server = server
        self.mic1 = mic1
        self.mic2 = mic2
        self.mic3 = mic3
        self.mics = (mic1, mic2, mic3)
        self.differential_drive = differential_drive
        self.gps = gps
        self.distress_call_finder = DistressCallFinder(self.mic1,
                                                       self.mic2,
                                                       self.mic3)
        self.ultrasonic_sensor1 = ultrasonic_sensor1
        self.ultrasonic_sensor2 = ultrasonic_sensor2
        self.ultrasonic_sensors = (ultrasonic_sensor1, ultrasonic_sensor2)
        self.PIR_sensor1 = PIR_sensor1
        self.PIR_sensor1 = PIR_sensor2
        self.PIR_sensors = (PIR_sensor1, PIR_sensor2)

        self.COMMAND_TO_FUNC = {'f': self.differential_drive.forward,
                                'b': self.differential_drive.backward,
                                'r': self.differential_drive.right,
                                'l': self.differential_drive.left}

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
        u<ultrasonic sensor index> -> return ultrasonic value
        
        G #TODO
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
        command_patterns = re.compile(r'^([GFRfbrlgui])(\d*)(D\d+)?$')
        matched = command_patterns.match(cmd)
        if matched:
            matched = matched.groups()

            ### Step 2: check for logical erros
            if matched[0] in self.COMMAND_TO_FUNC.keys():
                if not matched[1]:
                    self.print("Can't send a movement command without specifying how much!")
                    return None

                movement_value = int(matched[1])
                if movement_value == 0:
                    self.print("Can't move 0 anything!")
                    return None

                if matched[2]:
                    self.print("Movement Commands do not accept secondary parameter!")
                    return None

            if matched[0] in ['G', 'F', 'g']:
                if matched[1] or matched[2]:
                    self.print("Command doesn't support parameters")
                    return None

            if matched[0] == 'u':
                if not matched[1]:
                    # no need to check type is it will not be detected by regex!
                    self.print("Must Specify ultrasonic sensor index")
                    return None

                ultrasonic_index = int(matched[1])
                if ultrasonic_index < 0 or ultrasonic_index >= len(self.ultrasonic_sensors):
                    self.print(f"Max ultrasonic sensor index is {len(self.ultrasonic_sensors)-1}")
                    return None

            if matched[0] == 'i':
                if not matched[1]:
                    # no need to check type is it will not be detected by regex!
                    self.print("Must Specify PIR sensor index")
                    return None

                PIR_index = int(matched[1])
                if PIR_index < 0 or PIR_index >= len(self.PIR_sensors):
                    self.print(f"Max PIR sensor index is {len(self.PIR_sensors)-1}")
                    return None

            if matched[0] == 'R':
                if not matched[1]:
                    # no need to check type is it will not be detected by regex!
                    self.print("Must specific which mic to record from")
                    return None

                # Get mic_index
                mic_index = int(matched[1])
                if mic_index < 0 or mic_index >= len(self.mics):
                    self.print(f"Max Mic Index is {len(self.mics)-1}")
                    return None

                # Get Record duration
                if not matched[2]:
                    # no need to check type is it will not be detected by regex!
                    self.print("Must specific record duration in ms")
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

            elif matched[0] in self.COMMAND_TO_FUNC.keys():
                self.COMMAND_TO_FUNC[matched[0]](movement_value)
                self.print(f"Moving {self.COMMAND_TO_FUNC[matched[0]].__name__} for {int(matched[1])}{self.differential_drive.COMMAND_TO_UNIT[matched[0]]}")

            elif matched[0] == 'g':
                self.print(self.gps.raw_statements)

            elif matched[0] == 'F':
                self.print(f"Orientation: {self.distress_call_finder.find_orientation()}")

            elif matched[0] == 'u':
                try:
                    self.print(f"Ultrasonic Sensor <{matched[1]}>: {self.ultrasonic_sensors[ultrasonic_index].distance_cm()}cm")
                except OSError:
                    self.print("Ultrasonic Sensor <{matched[1]}> OUT OF RANGE!")

            elif matched[0] == 'i':
                self.print(f"PIR Sensor <{matched[1]}>: {self.PIR_sensors[PIR_index].obstacle_detected}")

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

 
