from machine import Pin

led = Pin(8, Pin.OUT)

proximity_sensor = Pin(7, Pin.IN)

def control():
    global led, proximity_sensor
    while True:
        led.value(proximity_sensor.value())
