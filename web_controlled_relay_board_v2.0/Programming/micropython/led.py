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
    NEOPIXEL_LIGHT_INTENSITY = 60
    def __init__(self):
        '''constructor'''
        if self.PLATFORM == 'rp2':
            self.led_pin = Pin("LED", Pin.OUT)

        elif self.PLATFORM == 'esp32':
            import neopixel
            self.np = neopixel.NeoPixel(Pin(48), 8)

        else:
            raise ValueError("Unknown platform")

        # in case havent' been defined before
        self.timer = None

    def on(self) -> bool:
        ''' turns built-in led on '''
        if self.PLATFORM == 'rp2':
            self.led_pin.on()

        elif self.PLATFORM == 'esp32':
            self.np[0] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
            self.np[1] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
            self.np[2] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
            self.np.write()

        else:
            raise ValueError("Unknown platform")

        return True

    def off(self) -> bool:
        ''' turns built-in led off and Turns off blinking '''
        if self.PLATFORM == 'rp2':
            self.led_pin.off()

        elif self.PLATFORM == 'esp32':
            self.np[0] = (0, 0, 0)
            self.np[1] = (0, 0, 0)
            self.np[2] = (0, 0, 0)
            self.np.write()

        else:
            raise ValueError("Unknown platform")

        return True

    def toggle(self) -> bool:
        if self.PLATFORM == 'rp2':
            self.led_pin.toggle()
            return True

        elif self.PLATFORM == 'esp32':
            if self.np[0][0] == 0:
                self.on()
            elif self.np[0][0] == self.NEOPIXEL_LIGHT_INTENSITY:
                self.off()
            else:
                raise ValueError("unknown toggle np value!")
            return True

        else:
            raise ValueError("Unknown platform")

    def blink(self) -> bool:
        ''' Starts blinking '''
        if self.timer:
            return True

        if self.PLATFORM == 'rp2':
            self.timer = Timer(period=self.BLINK_PERIOD, mode=Timer.PERIODIC, callback=lambda t:self.toggle())

        elif self.PLATFORM == 'esp32':
            self.timer = Timer(0)
            self.timer.init(period=self.BLINK_PERIOD, mode=Timer.PERIODIC, callback=lambda t: self.toggle())

        else:
            raise ValueError("Unknown platform")

        return True

    def stop_blink(self) -> bool:
        '''stops blinking and closes led'''
        if self.timer:
            self.timer.deinit()
            self.timer = None
        self.off()

        return True







