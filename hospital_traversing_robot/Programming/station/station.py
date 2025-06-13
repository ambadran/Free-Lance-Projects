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

    def __init__(self):

        self.SCK_PIN = const(18)
        self.MOSI_PIN = const(19)
        self.MISO_PIN = const(16)
        self.CSN_PIN = const(20)
        self.CE_PIN = const(21)

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

    def get_device_time_ms(self) -> int:
        '''
        returns
        '''
        self.send("T")

    def print_nrf_registers(self):
        '''

        '''
        self.send("N")

class DifferentialControl(Station):
    '''
    Abstraction to control the open loop differential control of the robot
    '''
    # def __init__(self, station: Station):
    #     self.station = Station
    def __init__(self):
        pass

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
        #TODO: wait until distance finished message from controller?

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
        #TODO: wait until distance finished message?

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
        #TODO: wait until distance finished message from controller?

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
        #TODO: wait until distance finished message from controller?

class GPS(Station):
    '''
    Abstraction to get gps stuff from 
    '''
    # def __init__(self, station: Station):
    #     self.station = Station
    def __init__(self):
        pass

    @property
    def all(self):
        self.send("G")  # same as Gi0

    @property
    def latitude(self):
        self.send("Gi1")

    @property
    def longitude(self):
        self.send("Gi2")

    @property
    def heading(self):
        self.send("Gi3")

    @property
    def time(self):
        self.send("Gi4")

class IMU(Station):
    """
    Abstraction to read IMU data
    """
    # def __init__(self, station: Station):
    #     self.station = Station
    def __init__(self):
        pass

    @property
    def orientation_all(self):
        self.send("M")  # same as Mi0

    @property
    def raw_accel_values(self):
        self.send("Mi1")

    @property
    def raw_gyro_values(self):
        self.send("Mi2")

    @property
    def raw_mag_values(self):
        self.send("Mi3")

    @property
    def accel_offset_values(self):
        self.send("Mi4")

    @property
    def gyro_offset_values(self):
        self.send("Mi5")

    @property
    def mag_offset_values(self):
        self.send("Mi6")

    @property
    def orientation_roll(self):
        self.send("Mi7")

    @property
    def orientation_pitch(self):
        self.send("Mi8")

    @property
    def orientation_yaw(self):
        self.send("Mi9")

    def unlock_yaw_measurement(self):
        self.send("Mi10")

    def unlock_yaw_measurement(self):
        self.send("Mi11")

    def reset_yaw_value(self):
        self.send("Mi12")

    def mpu6050_internal_registers(self):
        self.send("Mi13")

    def HMC5883L_internal_registers(self):
        self.send("Mi14")

class Ultrasonic(Station):
    '''
    Abstraction to control and read Ultrasonic sensor
    '''
    # def __init__(self, station: Station):
    #     self.station = Station
    def __init__(self):
        pass

    def stop_measurment(self):
        self.send("Ui-1")

    @property
    def distance_status(self):
        self.send("Ui0")

    def start_measurement(self):
        self.send("Ui1")

class ClosedLoopControl(Station):
    '''
    Construction to control closed loop control
    '''
    def __init__(self):
        pass

    def reset_idle(self):
        self.send("Ci-1")

    @property
    def status(self):
        self.send("Ci0")

    def execute_closed_loop_orient(self):
        self.send("Ci1j1")

    @property
    def yaw_setpoint(self):
        self.send("Ci3")

    @yaw_setpoint.setter
    def set_yaw_setpoint(self, yaw_setpoint: int):
        self.send(f"Ci2j{yaw_setpoint}")

class PathPlaning(Station):
    '''
    Abstraction to path plan between two location
    '''
    def __init__(self):
        pass

    def plan(self, start_loc: int, end_loc: int):
        self.send(f"Pi{start_loc}j{end_loc}")

    def execute(self):
        self.send("Ei1")

    @property
    def status(self):
        self.send("Ei0")

    def stop_executing(self):
        self.send("Ei-1")
