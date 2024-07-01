'''
Smart Watch Program
'''
from machine import I2C, Pin
from mpu6050 import MPU6050
from gps import GPS
from ssd1306 import SSD1306_I2C
from max30100 import MAX30100
from time import sleep_ms
import gc

i2c = I2C(1, sda=Pin(21), scl=Pin(22), freq=400_000)

mpu = MPU6050(0, 1, i2c)
mpu.activate_compl_filter()

heart_sensor = MAX30100(i2c)

gps = GPS()

display = SSD1306_I2C(128, 64, i2c)

def print_repl():
    '''
    prints all values to terminal where a python script will catch the values and send to firebase
    '''
    global mpu, gps, heart_sensor, display

    # Updating values
    heart_sensor.read_sensor()

    print(f"MPU: ({mpu.angles_absolute[0]},{mpu.angles_absolute[1]})")
    print(f"Heart: {round(heart_sensor.ir/325, 2)}BPM")
    print(f"GPS: {gps.value}")
    print('\n')

    gc.collect()

def print_display():
    '''
    prints all values on the OLED
    '''
    global mpu, gps, heart_sensor, display

    # Updating values
    heart_sensor.read_sensor()

    display.fill(0)
    display.text(f"MPU: ({mpu.angles_absolute[0]},{mpu.angles_absolute[1]})", 0, 0, 1)
    display.text(f"Heart: {round(heart_sensor.ir/325, 2)}BPM", 0, 12, 1)
    display.text(f"GPS:" , 0, 24, 1)
    display.text(gps.str_value, 0, 36, 1)
    display.show()

    gc.collect()


 
def main():
    ''' 
    Main Routine
    '''
    while True:
        print_repl()
        print_display()
        sleep_ms(100)

