'''
Abstraction to read the MIC
'''
from machine import Pin, ADC

class Mic:
    '''
    Abstraction to read the MIC
    '''
    DEFAULT_ADC_PIN = const(26)
    DEFAULT_SAMPLING_RATE = const(6000) 
    DEFAULT_SAMPLE_DURATION = const(6000)  # seconds

    def __init__(self, adc_pin=DEFAULT_ADC_PIN,
            sampling_rate=DEFAULT_SAMPLING_RATE,
            sampling_duration=DEFAULT_SAMPLE_DURATION):

        self.adc = ADC(Pin(adc_pin))
        self.sampling_rate = sampling_rate
        self.sampling_duration = sampling_duration

    def sample(self):
        '''
        returns a sample read
        '''
        return f"{self.adc.read_u16()}\n"

