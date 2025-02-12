'''
Abstraction for speaker control
'''
from machine import PWM, Pin
from time import sleep

class Speaker:
    def __init__(self, pin_num):
        self.pwm = PWM(Pin(pin_num, Pin.OUT), duty=0)

    def test(self):
        '''

        '''
        tempo = 4
        #notes and corresponding frequency
        tones = {
            'c': 262,
            'd': 294,
            'e': 330,
            'f': 349,
            'g': 392,
            'a': 440,
            'b': 494,
            'C': 523,
            ' ': 0,
        }
        melody = 'cdefggaaaagaaaagffffeeddddc'
        rhythm = [8, 8, 8, 8, 4, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 4, 8, 8, 8, 8, 4]

        # turning on the speaker
        self.pwm.duty(512)

        for tone, length in zip(melody, rhythm):
            self.pwm.freq(tones[tone])
            sleep(tempo/length)


        # turning off the speaker
        self.pwm.duty(0)


