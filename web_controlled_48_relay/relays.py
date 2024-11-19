'''
Abstraction to controls Relays
'''
from machine import Pin

class Relays:
    '''
    Abstraction to controls Relays
    '''
    DEFAULT_PIN_NUMS = [2, 
                        4]
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
            self.relays[pin_num].off()

        else:
            for relay in relays:
                relay.off()

    def on(self, pin_num):
        '''
        turns on a specific pin
        '''
        self.relays[pin_num].on()

    def value(self, pin_num: int, state: bool=None):
        '''
        setting a relay to a value
        '''
        if state is None:
            return self.relays[pin_num].value()
        else:
            self.relays[pin_num].value(state)

    def __getitem__(self, slicing: slice):
        '''
        slicing to access a specific Pin Object
        '''
        return self.relays[slicing]

    def __str__(self):
        '''
        return values of all the relays
        '''
        string = ""
        for pin_num, pin in enumerate(self.relays):
            string += "Pin {pin_num}: {pin.value()}\n"

        return string

