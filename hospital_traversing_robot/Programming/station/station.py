"""

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
    Central communication hub for robot control
    '''
    SPI_HARDWARE_INDEX = const(0)
    DEFAULT_PAYLOAD_SIZE = const(32)
    CHANNEL = const(46)
    PIPES = (b"\xe1\xf0\xf0\xf0\xf0", b"\xd2\xf0\xf0\xf0\xf0")
    RX_POLL_DELAY = const(15)
    DEFAULT_COMMAND_ACK_TIMEOUT = 3000  # 3 sec

    def __init__(self):
        # Pin definitions
        self.SCK_PIN = const(18)
        self.MOSI_PIN = const(19)
        self.MISO_PIN = const(16)
        self.CSN_PIN = const(20)
        self.CE_PIN = const(21)
        
        # Initialize SPI and radio
        self.spi = SPI(self.SPI_HARDWARE_INDEX, 
                      sck=Pin(self.SCK_PIN),
                      mosi=Pin(self.MOSI_PIN),
                      miso=Pin(self.MISO_PIN))
        self.csn = Pin(self.CSN_PIN, mode=Pin.OUT, value=1)
        self.ce = Pin(self.CE_PIN, mode=Pin.OUT, value=0)
        
        self.nrf = NRF24L01(self.spi, self.csn, self.ce, 
                           payload_size=self.DEFAULT_PAYLOAD_SIZE)
        self.nrf.open_tx_pipe(self.PIPES[0])
        self.nrf.open_rx_pipe(1, self.PIPES[1])
        self.nrf.set_channel(self.CHANNEL)
        self.nrf.start_listening()

        # Initialize components
        self.led_pin = Pin("LED", Pin.OUT)
        self.differential = DifferentialControl(self)
        self.gps = GPS(self)
        self.imu = IMU(self)
        self.ultrasonic = Ultrasonic(self)
        self.closed_loop = ClosedLoopControl(self)
        self.path_planning = PathPlanning(self)

        self.state = state.RECEIVER
        self.start_periodic_receive_timer()

    def start_periodic_receive_timer(self):
        self.timer = Timer(period=500, mode=Timer.PERIODIC, callback=self.periodic_receive)

    def stop_periodic_receive_timer(self):
        self.timer.deinit()

    def periodic_receive(self, t):
        self.led_pin.toggle()
        buf = self.receive()
        if buf:
            print(buf, end="")

    def receive(self) -> str:
        if self.state == state.RECEIVER and self.nrf.any():
            buf = ""
            while self.nrf.any():
                try:
                    buf += self.nrf.recv().decode()
                except UnicodeError:
                    pass  # Handle partial/invalid packets gracefully
            return buf

    def send(self, string="", timeout: Optional[int]=None):
        self.state = state.TRANSMITTER
        sleep_ms(20)
        self.nrf.stop_listening()
        self.nrf.send_ascii(string + '\n')
        self.nrf.start_listening()
        self.state = state.RECEIVER

        self.stop_periodic_receive_timer()
        response = ""
        timeout = timeout or self.DEFAULT_COMMAND_ACK_TIMEOUT
        start = ticks_ms()
        start_dots = ticks_ms()
        while "Command Passed" not in response and "Command Failed" not in response:
            buf = self.receive()
            if buf:
                response += buf
                start = ticks_ms()
            if ticks_diff(ticks_ms(), start) > timeout:
                print('\n', response, '\n')
                self.start_periodic_receive_timer()

                raise OSError(errno.ETIMEDOUT, "Didn't receive command acknowledgement from STC Microcontroller!")
            if ticks_diff(ticks_ms(), start_dots) > 800:
                print(".", end='')

        print('\n', response)
        self.start_periodic_receive_timer()

    def get_device_time_ms(self):
        self.send("T")

    def print_nrf_registers(self):
        self.send("N")

# Controller Components (using composition)
class DifferentialControl:
    def __init__(self, station):
        self.station = station

    def forward(self, distance: int, speed: int = None):
        cmd = f"Fi{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"
        self.station.send(cmd)

    def backward(self, distance: int, speed: int = None):
        cmd = f"Bi{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"
        self.station.send(cmd)

    def right(self, distance: int, speed: int = None):
        cmd = f"Ri{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"
        self.station.send(cmd)

    def left(self, distance: int, speed: int = None):
        cmd = f"Li{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"
        self.station.send(cmd)

class GPS:
    def __init__(self, station):
        self.station = station

    def all(self):
        self.station.send("G")

    def latitude(self):
        self.station.send("Gi1")

    def longitude(self):
        self.station.send("Gi2")

    def heading(self):
        self.station.send("Gi3")

    def time(self):
        self.station.send("Gi4")

class IMU:
    def __init__(self, station):
        self.station = station

    def orientation_all(self):
        self.station.send("M")

    def raw_accel_values(self):
        self.station.send("Mi1")

    # ... (other IMU methods follow same pattern)

class Ultrasonic:
    def __init__(self, station):
        self.station = station

    def stop_measurement(self):
        self.station.send("Ui-1")

    def distance_status(self):
        self.station.send("Ui0")

    def start_measurement(self):
        self.station.send("Ui1")

class ClosedLoopControl:
    def __init__(self, station):
        self.station = station

    def reset_idle(self):
        self.station.send("Ci-1")

    def status(self):
        self.station.send("Ci0")

    def execute_closed_loop_orient(self):
        self.station.send("Ci1j1")

    def yaw_setpoint(self):
        self.station.send("Ci3")

    def yaw_setpoint(self, value: int):
        self.station.send(f"Ci2j{value}")

class PathPlanning:
    def __init__(self, station):
        self.station = station

    def plan(self, start_loc: int, end_loc: int):
        self.station.send(f"Pi{start_loc}j{end_loc}")

    def execute(self):
        self.station.send("Ei1")

    def status(self):
        self.station.send("Ei0")

    def stop_executing(self):
        self.station.send("Ei-1")
