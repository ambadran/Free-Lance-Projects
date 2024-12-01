'''
This script controls DC motors. It constains the following abstractions:
    - Abstraction for DC Motor control through the l293 IC

    - Abstraction for both DC motors to be controlled to move the robot forward, backward, right, left

'''
from micropython import const
from machine import Pin, PWM, Timer

class DCMotor:
    '''
    Abstraction for DC Motor control through the l293 IC
    '''
    DEFAULT_FREQ = const(5000)  # max freq according to l293 datasheet
    DEFAULT_DUTY = const(65535)  # defaults to max
    def __init__(self, in1_pin, in2_pin, en_pin):
        self.in1 = Pin(in1_pin, Pin.OUT)
        self.in2 = Pin(in2_pin, Pin.OUT)
        self.en = PWM(Pin(en_pin, Pin.OUT), freq=self.DEFAULT_FREQ, duty_u16=DEFAULT_DUTY)

    def duty_cycle(self, percentage):
        '''
        sets duty cycle of PWM
        '''
        self.en.duty_u16(round((percentage/100)*65535))

    def cw(self):
        '''
        sets Motor to rotate clockwise
        '''
        self.in1.off()
        self.in2.on()

    def ccw(self):
        '''
        sets Motor to rotate anti-clockwise
        '''
        self.in1.on()
        self.in2.off()

    def off(self):
        '''
        shuts down motor by pulling down both pins
        '''
        self.in1.off()
        self.in2.off()


class DifferencialDrive:
    '''
    Abstraction for both DC motors to be controlled to move the robot
    forward, backward, right, left
    '''
    DEFAULT_IN1A_PIN = 1
    DEFAULT_IN1B_PIN = 16
    DEFAULT_EN1_PIN = 0
    DEFAULT_IN2A_PIN = 11
    DEFAULT_IN2B_PIN = 7
    DEFAULT_EN2_PIN = 12
    DEFAULT_LEFT_DC_MOTOR = DCMotor(
            in1_pin=DEFAULT_IN1A_PIN,
            in2_pin=DEFAULT_IN1B_PIN,
            en_pin=DEFAULT_EN1_PIN
            )
    DEFAULT_RIGHT_DC_MOTOR = DCMotor(
            in1_pin=DEFAULT_IN2A_PIN,
            in2_pin=DEFAULT_IN2B_PIN,
            en_pin=DEFAULT_EN2_PIN
            )

    #TODO: needs real-life testing
    ANGLE_TO_TIME_CONSTANT = 8.9  # how much milliseconds to turn 1 degree

    def __init__(self, left_motor: DCMotor=DEFAULT_LEFT_DC_MOTOR,
            right_motor: DCMotor=DEFAULT_RIGHT_DC_MOTOR):
        self.left_motor = left_motor
        self.right_motor = right_motor 
        self.stop()
        self.stop_timer = Timer(period=1, 
                mode=Timer.ONE_SHOT, 
                callback=lambda t:print("\nDifferencialDrive Control Initialization Done..\n"))  # Initializing Timer 

    def stop(self, t=0):
        '''
        immediately stop both motors
        '''
        self.left_motor.off()
        self.right_motor.off()

    def forward(self, duration: int):
        '''
        moves robot forward for {duration} milliseconds
        '''
        self.stop_timer.deinit()
        self.right_motor.cw()
        self.left_motor.cw()
        self.stop_timer = Timer(period=duration,
                                mode=Timer.ONE_SHOT, 
                                callback=self.stop)

    def backward(self, duration: int):
        '''
        moves robot backwards for {duration} milliseconds 
        '''
        self.stop_timer.deinit()
        self.right_motor.ccw()
        self.left_motor.ccw()
        self.stop_timer = Timer(period=duration, 
                                mode=Timer.ONE_SHOT, 
                                callback=self.stop)

    def right(self, angle: int):
        '''
        turns {angle} degrees to the right
        '''
        #TODO: take feedback from GPS module
        self.stop_timer.deinit()
        self.right_motor.ccw()
        self.left_motor.cw()
        self.stop_timer = Timer(period=round(angle*self.ANGLE_TO_TIME_CONSTANT), 
                mode=Timer.ONE_SHOT, 
                callback=self.stop)

    def left(self, angle: int):
        '''
        turns {angle} degrees to the left
        '''
        #TODO: take feedback from GPS module
        #TODO: check if angle is >0 and is int
        self.stop_timer.deinit()
        self.right_motor.cw()
        self.left_motor.ccw()
        self.stop_timer = Timer(period=round(angle*self.ANGLE_TO_TIME_CONSTANT), 
            mode=Timer.ONE_SHOT, 
            callback=self.stop)

    def orient(self, absolute_angle: int):
        '''
        turns left or right according integer sign
        '''
        if absolute_angle > 0:
            self.right(absolute_angle)
        else
            self.left(-absolute_angle)

