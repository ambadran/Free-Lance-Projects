'''
Abstraction to read the MIC
'''
from machine import Pin, ADC
from time import ticks_add, ticks_diff, ticks_ms, sleep
import gc

class Mic:
    '''
    Abstraction to read the MIC
    '''
    MIC1_ADC_PIN = const(26)
    MIC2_ADC_PIN = const(27)
    MIC3_ADC_PIN = const(28)
    DEFAULT_SAMPLING_RATE = const(14000) 
    DEFAULT_SAMPLE_DURATION = const(6000)  # seconds

    def __init__(self, adc_pin: int,
            sampling_rate=DEFAULT_SAMPLING_RATE,
            sampling_duration=DEFAULT_SAMPLE_DURATION):
        self.adc = ADC(Pin(adc_pin))
        self.sampling_rate = sampling_rate
        self.sample_time_ms = (1/sampling_rate)*1000
        self.sample_time_ms_2 = (1/sampling_rate)*500 # needed for the loop which counts in 2s
        self.sampling_duration = sampling_duration

    def pretty_sample(self) -> str:
        '''
        returns a sample read
        '''
        return f"{self.adc.read_u16()}\n"

    def avg_sample(self, samples:int=200) -> int:
        '''
        reads <samples> and takes average
        '''
        sum_value = self.adc.read_u16()
        for _ in range(samples-1):
            sum_value += self.adc.read_u16()

        return round(sum_value/samples)

    def get_samples(self) -> bytearray:
        '''
        reads a whole bunch of samples
        '''
        gc.collect()
        num_samples = round(self.sampling_rate*(self.sampling_duration/1000))
        bytearray_size = num_samples*2
        samples = bytearray(bytearray_size)
        print(f"Capturing {int(len(samples)/2)} samples in {self.sampling_duration} ms audio..\n")

        s_time = ticks_ms()
        for ind in range(0, bytearray_size, 2):
            value = self.adc.read_u16()
            samples[ind] = value & 0xFF  # Lower byte
            samples[ind+1] = (value >> 8) & 0xFF  # Upper byte

            while ticks_ms() < (s_time+ind*self.sample_time_ms_2):
                pass  # wait :)

        e_time = ticks_diff(ticks_ms(), s_time)

        print(f"Read all samples in {e_time}ms!")
        print(f"Sample Rate: {round((int(len(samples)/2))/(e_time/1000))}\n\n")

        gc.collect()
        print(f"Free Memory: {gc.mem_free()}")
        print()

        return samples

    def max_record_length(self) -> int:
        '''
        return max millisecond possible of record duration according to memory availability
        '''
        #TODO:
        return 5000


