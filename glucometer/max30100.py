""""
  Library for the Maxim MAX30100 pulse oximetry system on Raspberry Pi

  Based on original C library for Arduino by Connor Huffine/Kontakt
  https: // github.com / kontakt / MAX30100

  September 2017
"""

INT_STATUS   = const(0x00)  # Which interrupts are tripped
INT_ENABLE   = const(0x01)  # Which interrupts are active
FIFO_WR_PTR  = const(0x02)  # Where data is being written
OVRFLOW_CTR  = const(0x03)  # Number of lost samples
FIFO_RD_PTR  = const(0x04)  # Where to read from
FIFO_DATA    = const(0x05)  # Ouput data buffer
MODE_CONFIG  = const(0x06)  # Control register
SPO2_CONFIG  = const(0x07)  # Oximetry settings
LED_CONFIG   = const(0x09)  # Pulse width and power of LEDs
TEMP_INTG    = const(0x16)  # Temperature value, whole number
TEMP_FRAC    = const(0x17)  # Temperature value, fraction
REV_ID       = const(0xFE)  # Part revision
PART_ID      = const(0xFF)  # Part ID, normally 0x11

I2C_ADDRESS  = const(0x57)  # I2C address of the MAX30100 device


PULSE_WIDTH = {
    200: 0,
    400: 1,
    800: 2,
   1600: 3,
}

SAMPLE_RATE = {
    50: 0,
   100: 1,
   167: 2,
   200: 3,
   400: 4,
   600: 5,
   800: 6,
  1000: 7,
}

LED_CURRENT = {
       0: 0,
     4.4: 1,
     7.6: 2,
    11.0: 3,
    14.2: 4,
    17.4: 5,
    20.8: 6,
    24.0: 7,
    27.1: 8,
    30.6: 9,
    33.8: 10,
    37.0: 11,
    40.2: 12,
    43.6: 13,
    46.8: 14,
    50.0: 15
}

def _get_valid(d, value):
    try:
        return d[value]
    except KeyError:
        raise KeyError("Value %s not valid, use one of: %s" % (value, ', '.join([str(s) for s in d.keys()])))

def _twos_complement(val, bits):
    """compute the 2's complement of int value val"""
    if (val & (1 << (bits - 1))) != 0: # if sign bit is set e.g., 8bit: 128-255
        val = val - (1 << bits)
    return val

INTERRUPT_SPO2 = 0
INTERRUPT_HR = 1
INTERRUPT_TEMP = 2
INTERRUPT_FIFO = 3

MODE_HR = 0x02
MODE_SPO2 = 0x03


class MAX30100(object):

    def __init__(self,
                 i2c,
                 mode=MODE_HR,
                 sample_rate=100,
                 led_current_red=11.0,
                 led_current_ir=11.0,
                 pulse_width=1600,
                 max_buffer_len=10000
                 ):

        # Default to the standard I2C bus on Pi.
        # self.i2c = i2c if i2c else smbus.SMBus(1)
        self.i2c = i2c

        self.set_mode(MODE_HR)  # Trigger an initial temperature read.
        self.set_led_current(led_current_red, led_current_ir)
        self.set_spo_config(sample_rate, pulse_width)

        # Reflectance data (latest update)
        self.buffer_red = []
        self.buffer_ir = []

        self.max_buffer_len = max_buffer_len
        self._interrupt = None

    @property
    def red(self):
        return self.buffer_red[-1] if self.buffer_red else None

    @property
    def ir(self):
        return self.buffer_ir[-1] if self.buffer_ir else None

    def set_led_current(self, led_current_red=11.0, led_current_ir=11.0):
        # Validate the settings, convert to bit values.
        led_current_red = _get_valid(LED_CURRENT, led_current_red)
        led_current_ir = _get_valid(LED_CURRENT, led_current_ir)
        self.i2c.writeto_mem(I2C_ADDRESS, LED_CONFIG, chr((led_current_red << 4) | led_current_ir).encode())

    def set_mode(self, mode):
        reg = ord(self.i2c.readfrom_mem(I2C_ADDRESS, MODE_CONFIG, 1))
        self.i2c.writeto_mem(I2C_ADDRESS, MODE_CONFIG, chr(reg & 0x74).encode())  # mask the SHDN bit
        self.i2c.writeto_mem(I2C_ADDRESS, MODE_CONFIG, chr(reg | mode).encode())

    def set_spo_config(self, sample_rate=100, pulse_width=1600):
        reg = ord(self.i2c.readfrom_mem(I2C_ADDRESS, SPO2_CONFIG, 1))
        reg = reg & 0xFC  # Set LED pulsewidth to 00
        self.i2c.writeto_mem(I2C_ADDRESS, SPO2_CONFIG, chr(reg | pulse_width).encode())

    def enable_spo2(self):
        self.set_mode(MODE_SPO2)

    def disable_spo2(self):
        self.set_mode(MODE_HR)

    def enable_interrupt(self, interrupt_type):
        self.i2c.writeto_mem(I2C_ADDRESS, INT_ENABLE, chr((interrupt_type + 1)<<4).encode())
        self.i2c.readfrom_mem(I2C_ADDRESS, INT_STATUS, 1)

    def get_number_of_samples(self):
        write_ptr = ord(self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_WR_PTR, 1))
        read_ptr = ord(self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_RD_PTR, 1))
        return abs(16+write_ptr - read_ptr) % 16

    def read_sensor(self):
        bytes = self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_DATA, 4)
        # Add latest values.
        self.buffer_ir.append(bytes[0]<<8 | bytes[1])
        self.buffer_red.append(bytes[2]<<8 | bytes[3])
        # Crop our local FIFO buffer to length.
        self.buffer_red = self.buffer_red[-self.max_buffer_len:]
        self.buffer_ir = self.buffer_ir[-self.max_buffer_len:]

    def shutdown(self):
        reg = ord(self.i2c.readfrom_mem(I2C_ADDRESS, MODE_CONFIG, 1))
        self.i2c.writeto_mem(I2C_ADDRESS, MODE_CONFIG, chr(reg | 0x80).encode())

    def reset(self):
        reg = ord(self.i2c.readfrom_mem(I2C_ADDRESS, MODE_CONFIG, 1))
        self.i2c.writeto_mem(I2C_ADDRESS, MODE_CONFIG, chr(reg | 0x40).encode())

    def refresh_temperature(self):
        reg = ord(self.i2c.readfrom_mem(I2C_ADDRESS, MODE_CONFIG, 1))
        self.i2c.writeto_mem(I2C_ADDRESS, MODE_CONFIG, chr(reg | (1 << 3)).encode())

    def get_temperature(self):
        intg = _twos_complement(ord(self.i2c.readfrom_mem(I2C_ADDRESS, TEMP_INTG), 1))
        frac = ord(self.i2c.readfrom_mem(I2C_ADDRESS, TEMP_FRAC, 1))
        return intg + (frac * 0.0625)

    def get_rev_id(self):
        return ord(self.i2c.readfrom_mem(I2C_ADDRESS, REV_ID, 1))

    def get_part_id(self):
        return ord(self.i2c.readfrom_mem(I2C_ADDRESS, PART_ID, 1))

    def get_registers(self):
        return {
            "INT_STATUS": ord(self.i2c.readfrom_mem(I2C_ADDRESS, INT_STATUS, 1)),
            "INT_ENABLE": ord(self.i2c.readfrom_mem(I2C_ADDRESS, INT_ENABLE, 1)),
            "FIFO_WR_PTR": ord(self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_WR_PTR, 1)),
            "OVRFLOW_CTR": ord(self.i2c.readfrom_mem(I2C_ADDRESS, OVRFLOW_CTR, 1)),
            "FIFO_RD_PTR": ord(self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_RD_PTR, 1)),
            "FIFO_DATA": ord(self.i2c.readfrom_mem(I2C_ADDRESS, FIFO_DATA, 1)),
            "MODE_CONFIG": ord(self.i2c.readfrom_mem(I2C_ADDRESS, MODE_CONFIG, 1)),
            "SPO2_CONFIG": ord(self.i2c.readfrom_mem(I2C_ADDRESS, SPO2_CONFIG, 1)),
            "LED_CONFIG": ord(self.i2c.readfrom_mem(I2C_ADDRESS, LED_CONFIG, 1)),
            "TEMP_INTG": ord(self.i2c.readfrom_mem(I2C_ADDRESS, TEMP_INTG, 1)),
            "TEMP_FRAC": ord(self.i2c.readfrom_mem(I2C_ADDRESS, TEMP_FRAC)),
            "REV_ID": ord(self.i2c.readfrom_mem(I2C_ADDRESS, REV_ID, 1)),
            "PART_ID": ord(self.i2c.readfrom_mem(I2C_ADDRESS, PART_ID, 0)),
        }


