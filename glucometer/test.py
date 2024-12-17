from machine import Pin, I2C
from max30102 import MAX30102
from time import sleep_ms

i2c = I2C(1, sda=Pin(14), scl=Pin(15))

sensor = MAX30102(i2c=i2c)

# Setup with default values
sensor.setup_sensor()

def test():
    global sensor
    while (True):
        # The check() method has to be continuously polled, to check if
        # there are new readings into the sensor's FIFO queue. When new
        # readings are available, this function will put them into the storage.
        sensor.check()

        # Check if the storage contains available samples
        if (sensor.available()):
            # Access the storage FIFO and gather the readings (integers)
            red_sample = sensor.pop_red_from_storage()
            ir_sample = sensor.pop_ir_from_storage()

            # Print the acquired data (can be plot with Arduino Serial Plotter)
            print(red_sample, ",", ir_sample)

