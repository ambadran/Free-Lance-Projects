"""
Main Routine
"""
from micropython import const
from machine import Pin, SPI, Timer
from nrf24l01 import NRF24L01
from time import sleep_ms, sleep_us, ticks_ms, ticks_diff
import _thread
import errno


class state:
    RECEIVER = 0
    TRANSMITTER = 1

class Station:
    '''
    Abstraction for Station that will send commands and receive data
    '''
    SPI_HARDWARE_INDEX = const(0)
    DEFAULT_PAYLOAD_SIZE = const(32)
    CHANNEL = const(46)
    PIPES = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")  # Addresses are in little-endian format. They correspond to big-endian
    RX_POLL_DELAY = const(15)

    DEFAULT_COMMAND_ACK_TIMEOUT = 3000  # 2 sec

    def __init__(self, dev):

        if dev == 'pico':
            # Pico pinout
            self.SCK_PIN = const(18)
            self.MOSI_PIN = const(19)
            self.MISO_PIN = const(16)
            self.CSN_PIN = const(20)
            self.CE_PIN = const(21)

        elif dev == 'zero':
            # raspberry pi zero pinout
            self.SCK_PIN = const(2)
            self.MOSI_PIN = const(3)
            self.MISO_PIN = const(4)
            self.CSN_PIN = const(8)
            self.CE_PIN = const(14)

        else:
            raise ValueError("unsupported platform")

        self.spi = SPI(self.SPI_HARDWARE_INDEX, sck=Pin(self.SCK_PIN), mosi=Pin(self.MOSI_PIN), miso=Pin(self.MISO_PIN))
        self.csn = Pin(self.CSN_PIN, mode=Pin.OUT, value=1)
        self.ce = Pin(self.CE_PIN, mode=Pin.OUT, value=0)

        self.nrf = NRF24L01(self.spi, self.csn, self.ce, payload_size=self.DEFAULT_PAYLOAD_SIZE)

        self.nrf.open_tx_pipe(self.PIPES[0])
        self.nrf.open_rx_pipe(1, self.PIPES[1])
        self.nrf.start_listening()

        self.led_pin = Pin("LED", Pin.OUT)

        self.state = state.RECEIVER
        self.start_periodic_receive_timer()


    def start_periodic_receive_timer(self):
        '''
        start a timer that checks receive periodically. So as to catch any transmittion from the STC microcontroller after receiving a command acknowledgement
        '''
        self.timer = Timer(period=500, mode=Timer.PERIODIC, callback=self.periodic_receive)

    def stop_periodic_receive_timer(self):
        '''
        Stop receive timer,
        useful when I am catching command acknowledgement after sending a command
        '''
        self.timer.deinit()

    def periodic_receive(self, t):
        '''
        periodict check of any STC transmissions
        '''
        self.led_pin.toggle()
        buf = self.receive()
        if buf:
            print(buf, end="")

    def receive(self) -> str:
        '''
        listens, prints if received and saves values in internal attributes
        '''

        if self.state == state.RECEIVER:
            if self.nrf.any():
                buf = ""
                while self.nrf.any():
                    buf += self.nrf.recv().decode()
                    # sleep_ms(500)
                return buf

    def keep_receiving(self):
        '''
        
        '''
        try:
            while True:
                buf = self.receive()
                if buf:
                    print(buf)
                sleep_us(self.RX_POLL_DELAY)
        except KeyboardInterrupt:
            pass

    def send(self, string="", timeout: Optional[int]=None):
        '''
        sends enter
        '''
        self.state = state.TRANSMITTER
        sleep_ms(20)
        self.nrf.send_ascii(string+'\n')
        self.state = state.RECEIVER

        self.stop_periodic_receive_timer()
        response = ""
        start = ticks_ms()
        if timeout == None:
            timeout = self.DEFAULT_COMMAND_ACK_TIMEOUT
        while "Command Passed" not in response and "Command Failed" not in response:
            buf = self.receive()
            if buf:
                response += buf
                start = ticks_ms()
            if ticks_diff(ticks_ms(), start) > timeout:
                print('\n', response, '\n')
                self.start_periodic_receive_timer()

                raise OSError(errno.ETIMEDOUT, "Didn't receive command acknowledgement from STC Microcontroller!")

            # sleep_us(self.RX_POLL_DELAY)
            sleep_ms(20)
            print(".", end='')
        print('\n', response)
        self.start_periodic_receive_timer()

    def forward(self, distance: int, speed: int=None):
        '''
        distance is in cm
        speed is a Percentage value that gets converted to 16-bit value 
            to be sent to as 16-bit duty cycle
        '''
        if speed is None:
            self.send(f"Fi{distance}")
        else:
            speed = int((speed/100)*65535)
            self.send(f"Fi{distance}j{speed}")

    def backward(self, distance: int, speed: int=None):
        '''
        distance is in cm
        speed is a Percentage value that gets converted to 16-bit value 
            to be sent to as 16-bit duty cycle
        '''
        if speed is None:
            self.send(f"Bi{distance}")
        else:
            speed = int((speed/100)*65535)
            self.send(f"Bi{distance}j{speed}")

    def right(self, distance: int, speed: int=None):
        '''
        distance is in cm
        speed is a Percentage value that gets converted to 16-bit value 
            to be sent to as 16-bit duty cycle
        '''
        if speed is None:
            self.send(f"Ri{distance}")
        else:
            speed = int((speed/100)*65535)
            self.send(f"Ri{distance}j{speed}")

    def left(self, distance: int, speed: int=None):
        '''
        distance is in cm
        speed is a Percentage value that gets converted to 16-bit value 
            to be sent to as 16-bit duty cycle
        '''
        if speed is None:
            self.send(f"Li{distance}")
        else:
            speed = int((speed/100)*65535)
            self.send(f"Li{distance}j{speed}")

    def print_nrf_registers(self):
        '''

        '''
        self.state = state.TRANSMITTER
        self.nrf.print_registers()
        self.state = state.RECEIVER



