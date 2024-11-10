'''
Abstraction to read the MIC
'''
from machine import Pin, ADC

class Mic:
    '''
    Abstraction to read the MIC
    '''
    DEFAULT_ADC_PIN = const(26)
    DEFAULT_SAMPLING_RATE = const(8000) 
    DEFAULT_SAMPLE_DURATION = const(7000)  # 7 seconds

    def __init__(self, adc_pin=DEFAULT_ADC_PIN,
            sampling_rate=DEFAULT_SAMPLING_RATE,
            sampling_duration=DEFAULT_SAMPLE_DURATION):

        self.adc = ADC(Pin(adc_pin))
        self.sampling_rate = sampling_rate
        self.sampling_duration = sampling_duration

        self.counter = 0

    def sample(self):
        '''
        returns a sample read
        '''
        self.counter += 1
        return f"{self.counter}: {self.adc.read_u16()}\n"

