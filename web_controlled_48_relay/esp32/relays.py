'''
Abstraction to controls Relays
'''
from machine import Pin, Timer
import json

class Relays:
    '''
    Abstraction to controls Relays
    '''
    DEFAULT_PIN_NUMS = [2, 4, 16, 17, 18, 19, 21, 22, 
                        13, 12, 14, 27, 26, 25, 33, 32]
    RELAY_NUM = const(16)  # amount or relays we have
    DEFAULT_SAVE_PERIOD = const(1000)

    def __init__(self, pin_nums: list[int]=DEFAULT_PIN_NUMS):

        # Initializing the Pins and their retreived saved values
        self.__relays = []
        self.relay_values = self.retrieve_values() # to speed up read
        for pin_num, value in zip(pin_nums, self.relay_values):
            self.__relays.append(Pin(pin_num, Pin.OUT, value=value))

        # Running period timer to periodically save latest values
        # in flash memory
        self.timer = Timer(0, period=self.DEFAULT_SAVE_PERIOD, mode=Timer.PERIODIC, callback=self.save_values)
        
    def retrieve_values(self) -> list[int]:
        '''
        retrieve saved relay values and return list of relay values 
        '''
        try:
            with open('relay_values.json', 'r') as json_file:
                json_content = json_file.read()
            return json.loads(json_content)

        except OSError:
            print("Error Reading relay_values, returning zeros")
            #TODO: tell the user through tcp
            return [0]*self.RELAY_NUM

    def save_values(self, stupid_timer_arg=None):
        '''
        Save the current values to json file to involatile memory retrieval
        '''
        with open('relay_values.json', 'w') as json_file:
            json_file.write(json.dumps(self.values()))

    def off(self, pin_num=None):
        '''
        Turn all relays off
        '''
        if pin_num is None:
            for relay in self.__relays:
                relay.off()
        else:
            self.__relays[pin_num].off()

    def on(self, pin_num):
        '''
        turns on a specific pin
        '''
        self.__relays[pin_num].on()

    def value(self, pin_num: int, state: int=None) -> int:
        '''
        setting a relay to a value and returning the value of the relay in any case
        '''
        if state is None:
            # only returning current value
            return self.__relays[pin_num].value()

        else:
            # setting value
            self.__relays[pin_num].value(state)

            # returning new set value
            return self.__relays[pin_num].value()

    def values(self, states: list[int]=None) -> list[int]:
        '''
        reads ALL relay values
        or
        sets ALL relay values
        '''
        if states is None:
            # reading ALL relay values
            values = []
            for pin in self.__relays:
                values.append(pin.value())

            return values

        else:
            #TODO: check for type and length of states
            for pin_num, state in enumerate(states):
                self.__relays[pin_num].value(state)

            return self.values() 

    def __getitem__(self, slicing: slice):
        '''
        slicing to access a specific Pin Object
        '''
        return self.__relays[slicing]

    def __len__(self):
        '''
        returns how much relays on this board
        '''
        return self.RELAY_NUM

    def __str__(self):
        '''
        return values of all the relays
        '''
        string = ""
        for pin_num, value in enumerate(self.values()):
            string += f"R{pin_num}V{pin.value()}\n"

        return string

