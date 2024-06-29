'''
Smart Watch Program
'''
from machine import I2C, Pin
from mpu6050 import MPU6050
from gps import GPS
from ssd1306 import SSD1306_I2C

i2c = I2C(1, sda=Pin(21), scl=Pin(22), freq=400_000)

mpu = MPU6050(0, 1, i2c)
mpu.activate_compl_filter()

gps = GPS()

display = SSD1306_I2C(128, 64, i2c)
 
def main():
    ''' 
    Main Routine
    '''
    global mpu, gps, display

    while True:
        display.fill(0)
        display.text(f"MPU: ({mpu.angles_absolute[0]},{mpu.angles_absolute[1]})", 0, 0, 1)
        display.text(f"Heart BPM: {87}", 0, 12, 1)
        display.text(f"GPS:" , 0, 24, 1)
        display.text(str(gps.value), 0, 36, 1)
        display.show()


