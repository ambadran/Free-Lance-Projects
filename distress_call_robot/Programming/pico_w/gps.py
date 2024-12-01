"""
GPS NMEA Parser
"""
from micropython import const
from machine import UART, Pin
from time import ticks_add, ticks_ms, ticks_diff

class GPS:
    '''
    GPS NMEA Parser
    '''
    DEFAULT_TX_PIN = const(8)
    DEFAULT_RX_PIN = const(9)
    DEFAULT_BAUD_RATE = const(9600)

    NMEA_CMD = ['GPRMC', 'GPVTG', 'GPGGA', 'GPGSA', 'GPGSV']

    def __init__(self, tx_pin=DEFAULT_TX_PIN, rx_pin=DEFAULT_RX_PIN):
        self.uart = UART(1, baudrate=DEFAULT_BAUD_RATE, tx=Pin(tx_pin), rx=Pin(rx_pin))

    @property
    def raw_statements(self):
        '''
        return raw statements from the Neo 7m GPS
        '''
        timeout = ticks_add(ticks_ms(), 1000)
        while ticks_diff(timeout, ticks_ms()) > 0:
            if self.uart.any():
                buf = self.uart.read()
                return buf.decode()
        return ''


# gps = GPS()

