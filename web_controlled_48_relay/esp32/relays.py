'''
Abstraction to controls Relays
'''
from machine import Pin

class Relays:
    '''
    Abstraction to controls Relays
    '''
    DEFAULT_PIN_NUMS = [2, 4, 16, 17, 18, 19, 21, 22, 
                        13, 12, 14, 27, 26, 25, 33, 32]
    def __init__(self, pin_nums: list[int]=DEFAULT_PIN_NUMS):

        # Initializing the Pins
        self.relays = []
        for pin_num in pin_nums:
            self.relays.append(Pin(pin_num, Pin.OUT))

        self.off()

    def off(self, pin_num=None):
        '''
        Turn all relays off
        '''
        if pin_num is None:
            for relay in self.relays:
                relay.off()
        else:
            self.relays[pin_num].off()

    def on(self, pin_num):
        '''
        turns on a specific pin
        '''
        self.relays[pin_num].on()

    def value(self, pin_num: int, state: int=None) -> int:
        '''
        setting a relay to a value and returning the value of the relay in any case
        '''
        if state is None:
            # only returning current value
            return self.relays[pin_num].value()
        else:
            # setting value
            self.relays[pin_num].value(state)
            # and return value after being set
            return self.relays[pin_num].value()

    def __getitem__(self, slicing: slice):
        '''
        slicing to access a specific Pin Object
        '''
        return self.relays[slicing]

    def __len__(self):
        '''
        returns how much relays on this board
        '''
        return len(self.relays)

    def __str__(self):
        '''
        return values of all the relays
        '''
        string = ""
        for pin_num, pin in enumerate(self.relays):
            string += f"R{pin_num}V{pin.value()}\n"

        return string

