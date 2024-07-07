'''
This script running on Raspberry Pi Zero is supposed to Control the Smart Product Display Base. It is supposed to:
    - Detect Product Presence by interfacing IR Proximity Sensors through the GPIOs (HIGH/LOW signal)
    - Control Product Base LED lighting. (HIGH/LOW signal)
    - View Corresponding Videos through HDMI.
'''
from typing import Optional
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

class SmartProductDisplayBase:
    '''
    Object to abstract the sensor reading, led controlling and video viewing of one Product Display Base
    '''
    DEFAULT_PIN_NUM_SENSOR1 = 4
    DEFAULT_PIN_NUM_SENSOR2 = 17
    DEFAULT_PIN_NUM_SENSOR3 = 27
    DEFAULT_PIN_NUM_SENSOR4 = 22
    DEFAULT_PIN_NUM_SENSOR5 = 10
    DEFAULT_PIN_NUM_SENSOR6 = 9

    DEFAULT_PIN_NUM_LED1 = 18
    DEFAULT_PIN_NUM_LED2 = 23
    DEFAULT_PIN_NUM_LED3 = 24
    DEFAULT_PIN_NUM_LED4 = 25
    DEFAULT_PIN_NUM_LED5 = 8
    DEFAULT_PIN_NUM_LED6 = 7

    DEFAULT_VIDEO_PATH1 = './videos/video1.mp4'
    DEFAULT_VIDEO_PATH2 = './videos/video2.mp4'
    DEFAULT_VIDEO_PATH3 = './videos/video3.mp4'
    DEFAULT_VIDEO_PATH4 = './videos/video4.mp4'
    DEFAULT_VIDEO_PATH5 = './videos/video5.mp4'
    DEFAULT_VIDEO_PATH6 = './videos/video6.mp4'

    BOOL_TO_GPIO_VALUE = {False: GPIO.LOW, True: GPIO.HIGH}
    def __init__(self, proximity_sensor_pin_num: int, led_pin_num: int, video_path: str):

        GPIO.setmode(GPIO.BCM)

        GPIO.setup(proximity_sensor_pin_num, GPIO.IN)
        self.proximity_sensor_pin_num = proximity_sensor_pin_num

        GPIO.setup(led_pin_num, GPIO.OUT)
        self.led_pin_num = led_pin_num
        self.led_value = GPIO.LOW  # and sets the internal ._led_value

        self.video_path = video_path
        self.video_playing = False

    @property
    def product_present(self) -> bool:
        '''
        returns whether the product is detected or not
        '''
        return bool(GPIO.input(self.proximity_sensor_pin_num))

    @property
    def led_value(self) -> int:
        '''
        return the led value [GPIO.LOW, GPIO.HIGH]
        '''
        return self._led_value

    @led_value.setter
    def led_value(self, value: int):
        '''
        sets the led value
        '''
        if value in [GPIO.LOW, GPIO.HIGH]:
            GPIO.output(self.led_pin_num, value)
            self._led_value = value
        else:
            raise ValueError("unknown value argument passed, should be GPIO.LOW or GPIO.HIGH")

      def stop_video(self):
        '''
        Stops video playing
        '''
        os.system("pkill oxmplayer")

      def play_video(self):
        '''
        Starts video playing
        '''
        os.system(f"omxplayer --loop --no-osd {self.video_path} &")

def main():
    '''
    Main Routine
    '''
    try:
        ### INITs ###
        smart_product_display_base1 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR1,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED1,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH1
                )
        smart_product_display_base2 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR2,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED2,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH2
                )
        smart_product_display_base3 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR3,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED3,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH3
                )
        smart_product_display_base4 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR4,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED4,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH4
                )
        smart_product_display_base5 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR5,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED5,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH5
                )
        smart_product_display_base6 = SmartProductDisplayBase(
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_SENSOR6,
                    SmartProductDisplayBase.DEFAULT_PIN_NUM_LED6,
                    SmartProductDisplayBase.DEFAULT_VIDEO_PATH6
                )

        ### Main Loop ###
        while True:

          # Setting LEDs
          smart_product_display_base1.led_value = smart_product_display_base1.BOOL_TO_GPIO_VALUE[smart_product_display_base1.product_present]
          smart_product_display_base2.led_value = smart_product_display_base2.BOOL_TO_GPIO_VALUE[smart_product_display_base2.product_present]
          smart_product_display_base3.led_value = smart_product_display_base3.BOOL_TO_GPIO_VALUE[smart_product_display_base3.product_present]
          smart_product_display_base4.led_value = smart_product_display_base4.BOOL_TO_GPIO_VALUE[smart_product_display_base4.product_present]
          smart_product_display_base5.led_value = smart_product_display_base5.BOOL_TO_GPIO_VALUE[smart_product_display_base5.product_present]
          smart_product_display_base6.led_value = smart_product_display_base6.BOOL_TO_GPIO_VALUE[smart_product_display_base6.product_present]

          # Setting Video
          #TODO:
          current_video_playing: Optional[None, int] = None
          if not smart_product_display_base1.product_present:
            if not smart_product_display_base1.video_playing:





