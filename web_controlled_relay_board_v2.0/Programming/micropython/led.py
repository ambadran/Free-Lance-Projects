"""
Led Management
"""
import sys
from machine import Pin, Timer

class DebugLED:
    '''
    Abstraciton to initialize and blink/set LED using timer library
    '''
    PLATFORM = sys.platform
    BLINK_PERIOD = 500
    def __init__(self):
        '''constructor'''
        if self.PLATFORM == 'rp2':
            self.led_pin = Pin("LED", Pin.OUT)

        elif self.PLATFORM == 'esp32':
            import neopixel
            self.led_pin = neopixel.NeoPixel(Pin(48), 8)

        else:
            raise ValueError("Unknown platform")

        # in case havent' been defined before
        self.timer = None

    def on(self):
        ''' turns built-in led on '''
        if self.PLATFORM == 'rp2':
            self.led_pin.on()

        elif self.PLATFORM == 'esp32':
            self.np[0] = (255, 255, 255)
            self.np[1] = (255, 255, 255)
            self.np[2] = (255, 255, 255)
            self.np.write()

        else:
            raise ValueError("Unknown platform")

    def off(self):
        ''' turns built-in led off and Turns off blinking '''
        if self.timer:
            self.timer.deinit()

        if self.PLATFORM == 'rp2':
            self.led_pin.off()

        elif self.PLATFORM == 'esp32':
            self.np[0] = (0, 0, 0)
            self.np[1] = (0, 0, 0)
            self.np[2] = (0, 0, 0)
            self.np.write()

        else:
            raise ValueError("Unknown platform")

    def toggle(self):
        if self.PLATFORM == 'rp2':
            self.led_pin.toggle()

        elif self.PLATFORM == 'esp32':
            if self.np[0][0] == 0:
                self.on()
            elif self.[0][0] == 255:
                self.off()
            else:
                raise ValueError("unknown toggle np value!")

        else:
            raise ValueError("Unknown platform")

    def blink(self):
        if self.PLATFORM == 'rp2':
            self.timer = Timer(period=self.BLINK_PERIOD, mode=Timer.PERIODIC, callback=lambda t:self.toggle())

        elif self.PLATFORM == 'esp32':
            if self.np[0][0] == 0:
                self.on()
            elif self.[0][0] == 255:
                self.off()
            else:
                raise ValueError("unknown toggle np value!")

        else:
            raise ValueError("Unknown platform")







