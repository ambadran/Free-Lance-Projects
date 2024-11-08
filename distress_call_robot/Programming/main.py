'''
Main Routine
'''
from micropython import const
from machine import Pin, ADC
from station import Server
from time import sleep

SAMPLING_RATE = const(8000)

mic = ADC(Pin(26))
server = Server()

# try:
#TODO: implement counter to check for missing values
counter = 0
server.wait_for_client()
while True:
    value = f"0: {mic.read_u16()}\n"
    server.client.send(value)
    sleep(1/SAMPLING_RATE)

# except Exception as e:
#     print()
#     server.client.close()

