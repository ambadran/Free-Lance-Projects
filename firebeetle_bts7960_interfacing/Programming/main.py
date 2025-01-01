from machine import Pin, Timer

DISTANCE_MAX = const(5)
#TODO: needs tuning
CM_TO_MS = const(1500)  # 1cm takes 1500ms 

timer = Timer(0)
timer2 = Timer(1)
l_pin = Pin(32, Pin.OUT)
r_pin = Pin(18, Pin.OUT)
motor_pins = [l_pin, r_pin]

limit_switch = Pin(27, Pin.IN, Pin.PULL_UP)
limit_switch.irq(lambda t: stop(), Pin.IRQ_FALLING)

def stop():
    '''
    halts all actions
    '''
    l_pin.off()
    r_pin.off()
    # print("Movement Done!")

def move(direction: int, distance_cm: int=DISTANCE_MAX):
    '''
    turns on the correct pin for the right amount of time to reach a specific distance in a specific direction
    '''
    global l_pin, r_pin, timer, DISTANCE_MAX, motor_pins, CM_TO_MS

    # Type Checking
    if direction not in [0, 1]:
        raise ValueError("Direction Parameter can only 0 or 1")

    if distance_cm <= 0 or distance_cm > DISTANCE_MAX:
        raise ValueError("distance Parameter out of range!")

    # Can't issue a movement order while motor is already moving!
    if l_pin.value() or r_pin.value():
        raise ValueError("Motor Already Moving!")

    # Setting correct pin
    motor_pins[direction].on()

    # setting timer to turn it off after time needed to move
    timer.init(period=distance_cm*CM_TO_MS, mode=Timer.ONE_SHOT, callback=lambda t: stop())

    print(f"Moving Dir: {direction}, distance: {distance_cm}")

    


    




