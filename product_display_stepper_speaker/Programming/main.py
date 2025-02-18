'''
Main Routine
'''
from micropython import const
from stepper import Stepper
from speaker import Speaker

### Pin Assignments ###
EN_PIN = const(15)
DIR_PIN1 = const(12)
STEP_PIN1 = const(13)
DIR_PIN2 = const(16)
STEP_PIN2 = const(14)

SPEAKER_LEFT_PIN = const(5)
SPEAKER_RIGHT_PIN = const(4)

### HAL Devices ###
st1 = Stepper(EN_PIN, DIR_PIN1, STEP_PIN1)
st2 = Stepper(EN_PIN, DIR_PIN2, STEP_PIN2)

speaker_l = Speaker(5)
speaker_r = Speaker(4)

def move_both(steps):
    st1.step(steps)
    st2.step(steps)
