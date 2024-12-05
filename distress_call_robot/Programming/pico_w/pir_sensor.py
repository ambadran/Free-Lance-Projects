'''
Abstraction for PIR sensor
'''
from machine import Pin

class PIRSensor:
    '''
    Abstraction for PIR sensor
    '''
    PIN_NUM1 = 6
    PIN_NUM2 = 10
    def __init__(self, pin_num: int):
        self.ir_sensor = Pin(pin_num, Pin.IN)

    @property
    def obstacle_detected(self) -> bool:
        '''
        returns whether or not an obstacle is detected
        '''
        return not self.ir_sensor.value()



