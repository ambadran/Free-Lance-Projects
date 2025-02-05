# beeper code from https://micropython-on-esp8266-workshop.readthedocs.io/en/latest/basics.html#beepers
from machine import Pin, PWM
from time import sleep

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

speaker_l = Pin(19, Pin.OUT)
speaker_r = Pin(18, Pin.OUT)

# Init Beeper without frequency.
beeper_l = PWM(speaker_l, duty_u16=32768)
beeper_r = PWM(speaker_r, duty_u16=32768)
melody = 'cdefggaaaagaaaagffffeeddddc'
rhythm = [8, 8, 8, 8, 4, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 8, 8, 8, 8, 4, 4, 8, 8, 8, 8, 4]

for tone, length in zip(melody, rhythm):
    beeper_l.freq(tones[tone])
    beeper_r.freq(tones[tone])
    sleep(tempo/length)

beeper_l.deinit()
beeper_r.deinit()
    
