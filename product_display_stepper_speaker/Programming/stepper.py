'''
This file contains Stepper class to control Stepper motors through a DRV8825 stepper driver
'''
from micropython import const
from machine import Pin, Timer

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
    DEFAULT_FREQ = 10000

    MM_TO_STEP = 1600

    def __init__(self, en_pin, dir_pin, step_pin):
        self.en_pin = Pin(en_pin, Pin.OUT)
        self.dir_pin = Pin(dir_pin, Pin.OUT)
        self.step_pin = Pin(step_pin, Pin.OUT)

        self.is_moving = False

        self.free()
        self.dir = 0

    @property
    def state(self) -> State:
        '''
        getter for motor state 

        please note that enable pin is active low
        0 for FREE
        1 for HOLD
        '''
        return not self.en_pin.value()

    @state.setter
    def state(self, state: State):
        '''
        setter for motor state

        please note that enable pin is active low
        0 for FREE
        1 for HOLD
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

    def stop_movement(self, t):
        '''
        stop step timer
        '''
        self.step_timer.deinit()
        self.stop_timer.deinit()

        self.step_timer.deinit()
        self.stop_timer.deinit()

        self.step_timer.deinit()
        self.stop_timer.deinit()

        self.step_timer.deinit()
        self.stop_timer.deinit()


        self.free()
        self.is_moving = False

    def step(self, steps: int, frequency: int=1000, direction: Dir=None):
        '''
        moves stepper 'steps' amount without changing dir unless parameter is given

        fires two timers, 
        First is PERIODIC and toggles the step pin continiously
        Second is ONE SHOT and stops the first timer.

        This is just to avoid the extra cpu time of polling whether the steps are finished or not
        '''
        if self.is_moving:
            print("Already Moving!")
        else:
            self.hold()

            self.step_timer = Timer(-1)
            self.step_timer.init(freq=frequency, mode=Timer.PERIODIC, callback=lambda t:self.step_pin.value(not self.step_pin.value()))
            self.is_moving = True

            self.stop_timer = Timer(-1)
            self.stop_timer.init(period=int((steps/frequency)*1000), mode=Timer.ONE_SHOT, callback=self.stop_movement)

    def __repr__(self) -> str:
        return f"Stepper State: {self.state}\nStepper Dir: {self.dir}\nStepper Moving: {self.is_moving}"

