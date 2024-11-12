'''
Abstraction to read the MIC
'''
from machine import Pin, ADC
from time import ticks_add, ticks_diff, ticks_ms, sleep

class Mic:
    '''
    Abstraction to read the MIC
    '''
    DEFAULT_ADC_PIN = const(26)
    DEFAULT_SAMPLING_RATE = const(5000) 
    DEFAULT_SAMPLE_DURATION = const(100)  # seconds

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

    def audio_sample(self):
        '''
        reads a whole bunch of samples
        '''
        print(f"Capturing {self.sampling_duration} sec audio..")

        # dividing the audio sample into 100ms sections
        samples = []
        end_time = ticks_add(ticks_ms(), self.mic.sampling_duration)
        while ticks_diff(end_time, ticks_ms()) > 0:
            samples.append(self.mic.sample())




