"""
Led Management using a non-blocking polling method.
"""
import sys
import time
from machine import Pin

class DebugLED:
    '''
    Abstraction to initialize and blink/set LED using a non-blocking poll method.
    '''
    PLATFORM = sys.platform
    BLINK_PERIOD_MS = 500 # The time between each toggle, in milliseconds
    NEOPIXEL_LIGHT_INTENSITY = 60

    def __init__(self):
        '''constructor'''
        if self.PLATFORM == 'rp2':
            self.led_pin = Pin("LED", Pin.OUT)
        elif self.PLATFORM == 'esp32':
            # This import is conditional to save memory on other platforms
            import neopixel
            self.np = neopixel.NeoPixel(Pin(48), 8)
        else:
            raise ValueError("Unknown platform")

        # State variables for non-blocking blink
        self.is_blinking = False
        self.last_toggle_time = 0

    def on(self) -> bool:
        ''' Turns the LED on solid and stops any blinking. '''
        self.is_blinking = False # Explicitly setting ON stops blinking
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
        ''' Turns the LED off and stops any blinking. '''
        self.is_blinking = False # Explicitly setting OFF stops blinking
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
        ''' Toggles the current state of the LED. For internal use by blink_poll. '''
        if self.PLATFORM == 'rp2':
            self.led_pin.toggle()
        elif self.PLATFORM == 'esp32':
            # Check the state of the first pixel to determine toggle direction
            if self.np[0][0] == 0:
                self.np[0] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
                self.np[1] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
                self.np[2] = (self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY, self.NEOPIXEL_LIGHT_INTENSITY)
            else:
                self.np[0] = (0, 0, 0)
                self.np[1] = (0, 0, 0)
                self.np[2] = (0, 0, 0)
            self.np.write()
        else:
            raise ValueError("Unknown platform")
        return True

    def blink(self) -> bool:
        ''' Enables blinking mode. The actual blink is handled by blink_poll(). '''
        if self.is_blinking:
            return True # Already in blinking mode
        self.is_blinking = True
        self.last_toggle_time = time.ticks_ms()
        return True

    def stop_blink(self) -> bool:
        ''' Disables blinking mode and turns the LED off. '''
        self.is_blinking = False
        self.off() # Ensure LED is in a known OFF state
        return True

    def blink_poll(self):
        """
        This method must be called repeatedly in the main loop.
        It handles the non-blocking blink logic using ticks_diff.
        """
        if not self.is_blinking:
            return

        now = time.ticks_ms()
        # Use ticks_diff for correct handling of timer overflow
        if time.ticks_diff(now, self.last_toggle_time) >= self.BLINK_PERIOD_MS:
            self.toggle()
            self.last_toggle_time = now

