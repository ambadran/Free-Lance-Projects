'''
This script running on Raspberry Pi Zero is supposed to Control the Smart Product Display Base. It is supposed to:
    - Detect Product Presence by interfacing IR Proximity Sensors through the GPIOs (HIGH/LOW signal)
    - Control Product Base LED lighting. (HIGH/LOW signal)
    - View Corresponding Videos through HDMI.
'''
from abc import ABC
import RPi.GPIO as GPIO
import os
import time

class Switch:
    '''
    Abstract to easily interface a switch
    '''
    DEFAULT_SWITCH_PIN_NUM = 16
    def __init__(self, pin_num):
        GPIO.setmode(GPIO.BCM)
        GPIO.setup(pin_num, GPIO.IN)
        self.pin_num = pin_num

    @property
    def value(self):
        '''
        returns current value of the switch
        '''
        return GPIO.input(self.pin_num)

class GPIO_Group(ABC):
    '''
    Abstract base Class to Abstract a group of input/output GPIOs
    '''
    def __init__(self, *pin_nums: list[int], gpio_type: int):
        GPIO.setmode(GPIO.BCM)

        for pin_num in pin_nums:
            GPIO.setup(pin_num, gpio_type)

        self.pin_nums = pin_nums
        self.len_pin_nums = len(pin_nums)

class ProximitySensors(GPIO_Group):
    '''
    Object to abstract the reading of the proximity sensors
    '''
    DEFAULT_PIN_NUM_SENSOR1 = 4
    DEFAULT_PIN_NUM_SENSOR2 = 17
    DEFAULT_PIN_NUM_SENSOR3 = 27
    DEFAULT_PIN_NUM_SENSOR4 = 22
    DEFAULT_PIN_NUM_SENSOR5 = 10
    DEFAULT_PIN_NUM_SENSOR6 = 9
    def __init__(self, *pin_nums):
        super().__init__(pin_nums, GPIO.IN)

    @property
    def values(self) -> list[bool]:
        '''
        returns a list of the sensor values
        '''
        values = []
        for pin_num in pin_nums:
            values.append(bool(GPIO.input(pin_num)))

        return values

class LEDs(GPIO_Group):
    '''
    Object to abstract controlling Relays
    '''
    DEFAULT_PIN_NUM_LED1 = 18
    DEFAULT_PIN_NUM_LED2 = 23
    DEFAULT_PIN_NUM_LED3 = 24
    DEFAULT_PIN_NUM_LED4 = 25
    DEFAULT_PIN_NUM_LED5 = 8
    DEFAULT_PIN_NUM_LED6 = 7
    def __init__(self, *pin_nums):
        super().__init__(pin_nums, GPIO.OUT)
        self.off()  # also initializes the internal ._led_values list

    def off(self):
        '''
        turns All LEDs off
        '''
        for pin_num in self.pin_nums:
            GPIO.output(pin_num, GPIO.LOW)

        self._led_values = [GPIO.LOW] * self.len_pin_nums
        print("All LEDs OFF")

    def on(self):
        '''
        turns All LEDs off
        '''
        for pin_num in self.pin_nums:
            GPIO.output(pin_num, GPIO.HIGH)

        self._led_values = [GPIO.HIGH] * self.len_pin_nums
        print("All LEDs ON")

    def flash(self, num_flashes: int=5, sleep_time: int=0.5):
        '''
        flashes all LEDs at the same time `num_flashes` times with `sleep_time` intervals in between
        '''
        for _ in range(num_flashes):
            self.on()
            time.sleep(sleep_time)
            self.off()
            time.sleep(sleep_time)

    @property
    def values(self) -> list[int]:
        '''
        returns the values of the internal ._led_values list
        '''
        return self._led_values

    @values.setter
    def values(self, led_values: list[int]):
        '''
        sets the entire ._led_values list
        #TODO: can't set the ._led_values by slicing this .values attribute
        '''
        if (type(led_values) == list) and (False not in [(state in [GPIO.HIGH, GPIO.LOW]) for state in led_values]):

            for led_pin_num, led_value in zip(self.pin_nums, led_values):
                GPIO.output(led_pin_num, led_value)

            self._led_values = led_values
        else:
            raise ValueError("led_values parameter must be list of GPIO.LOW or GPIO.HIGH")

    def __setitem__(self, index: int, value: int):
        '''
        Sets Individual LED value
        '''
        if value in [GPIO.LOW, GPIO.HIGH]:
            GPIO.output(self.pin_nums[index], value)
            self._led_values[index] = value
        else:
            raise ValueError("unknown value error")

    def __getitem__(self, index) -> int:
        '''
        return Gets the value of an individual LED
        '''
        return self._led_values[index]

def main():
    '''
    Main Routine
    '''
    try:

        ### Inits ###
        switch = Switch(Switch.DEFAULT_SWITCH_PIN_NUM)
        proximity_sensors = ProximitySensors(
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR1,
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR2,
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR3,
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR4,
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR5,
                ProximitySensors.DEFAULT_PIN_NUM_SENSOR6
                )
        leds = LEDs(
                LEDS.DEFAULT_PIN_NUM_LED1,
                LEDS.DEFAULT_PIN_NUM_LED2,
                LEDS.DEFAULT_PIN_NUM_LED3,
                LEDS.DEFAULT_PIN_NUM_LED4,
                LEDS.DEFAULT_PIN_NUM_LED5,
                LEDS.DEFAULT_PIN_NUM_LED6
                )


        print("All System Components Up!")
        leds.flash()

        ### Main Loop ###
            while True:
                

    except KeyboardInterrupt:
        print("Script interrupted by User")

    finally:
        GPIO.cleanup()

        video.kill

