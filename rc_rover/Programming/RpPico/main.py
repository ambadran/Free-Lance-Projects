"""
Main Routine
"""
from micropython import const
from machine import Pin, SPI
from nrf24l01 import NRF24L01
from time import sleep_ms

class Station:
    '''
    Abstraction for Station that will send commands and receive data
    '''
    SPI_HARDWARE_INDEX = const(0)
    DEFAULT_PAYLOAD_SIZE = const(16)
    CHANNEL = const(40)
    PIPES = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")  # Addresses are in little-endian format. They correspond to big-endian
    RX_POLL_DELAY = const(15)

    SCK_PIN = const(18)
    MOSI_PIN = const(19)
    MISO_PIN = const(16)
    CSN_PIN = const(21)
    CE_PIN = const(20)

    def __init__(self):
        self.spi = SPI(self.SPI_HARDWARE_INDEX, sck=Pin(self.SCK_PIN), mosi=Pin(self.MOSI_PIN), miso=Pin(self.MISO_PIN))
        self.csn = Pin(self.CSN_PIN, mode=Pin.OUT, value=1)
        self.ce = Pin(self.CE_PIN, mode=Pin.OUT, value=0)

        self.nrf = NRF24L01(self.spi, self.csn, self.ce, payload_size=self.DEFAULT_PAYLOAD_SIZE)

        self.nrf.open_tx_pipe(self.PIPES[0])
        self.nrf.open_rx_pipe(1, self.PIPES[1])
        self.nrf.start_listening()

    def receive(self) -> str:
        '''
        listens, prints if received and saves values in internal attributes
        '''
        if self.nrf.any():
            while self.nrf.any():
                buf = self.nrf.recv()
                print(buf, "received")

    def keep_receiving(self):
        '''
        
        '''
        while True:
            self.receive()
            sleep_ms(self.RX_POLL_DELAY)

    def test(self):
        '''
        sends enter
        '''
        # self.nrf.set_channel(self.AXIS_CHANNEL[axe_num])
        # sleep_ms(300)

        self.nrf.send_ascii_m("")

    def forward(self, distance_cm: int):
        '''
        send forward command in cm
        '''
        self.nrf.send_ascii_m(f"f{distance_cm}")

    def backward(self, distance_cm: int):
        '''
        send backward command in cm
        '''
        self.nrf.send_ascii_m(f"b{distance_cm}")

    def right(self, angle_deg: int):
        '''
        send right command in degree
        '''
        self.nrf.send_ascii_m(f"r{angle_deg}")

    def left(self, angle_deg: int):
        '''
        send left command in degree
        '''
        self.nrf.send_ascii_m(f"l{angle_deg}")


