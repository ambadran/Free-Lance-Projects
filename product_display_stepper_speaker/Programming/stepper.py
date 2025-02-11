'''
This file contains Stepper class to control Stepper motors through a DRV8825 stepper driver
'''
from machine import Pin

class State:
    FREE = 1
    HOLD = 0

class Dir:
    CW = 0
    CCW = 1

class Stepper:
    '''
    Class to control bipolar stepper motor through a A4988/DRV8825
    '''
    EN_PIN = const(15)
    DIR_PIN = const(12)
    STEP_PIN = const(13)

    MM_TO_STEP = 1600

    def __init__(self):
        self.en_pin = Pin(EN_PIN, Pin.OUT)
        self.dir_pin = Pin(DIR_PIN, Pin.OUT)
        self.step_pin = Pin(STEP_PIN, Pin.OUT)

        self.free()

    @property
    def state(self) -> State:
        '''
        getter for motor direction
        '''
        return not self.en_pin.value()

    @state.setter
    def state(self, state: State):
        '''
        setter for motor direction
        '''
        self.en_pin.value(not state)

    def hold(self):
        self.en_pin.value(0)

    def free(self):
        self.en_pin.value(1)

    @property
    def dir(self) -> Dir:
        '''
        getter for motor direction
        '''
        return self.dir_pin.value()

    @dir.setter
    def dir(self, direction: Dir):
        '''
        setter for motor direction
        '''
        self.dir_pin.value(direction)

    def step(self, steps: int, direction: Dir=None):
        '''
        moves stepper 'steps' amount without changing dir unless parameter is given
        '''
        #TODO:
        pass
