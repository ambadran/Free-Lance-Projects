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
    DEFAULT_SCK_PIN = const(18)
    DEFAULT_MOSI_PIN = const(19)
    DEFAULT_MISO_PIN = const(16)
    DEFAULT_CSN_PIN = const(20)
    DEFAULT_CE_PIN = const(21)

    def __init__(self):
        # Pin definitions
        self.SCK_PIN = self.DEFAULT_SCK_PIN
        self.MOSI_PIN = self.DEFAULT_MOSI_PIN
        self.MISO_PIN = self.DEFAULT_MISO_PIN
        self.CSN_PIN = self.DEFAULT_CSN_PIN
        self.CE_PIN = self.DEFAULT_CE_PIN
        
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
        self.timer = None 
        # to start from REPL
        # self.timer = True, then call start_periodic_receive_timer() or any send() to start
        # self.start_periodic_receive_timer()

    def start_periodic_receive_timer(self):
        if self.timer:
            self.timer = Timer(period=500, mode=Timer.PERIODIC, callback=self.periodic_receive)

    def stop_periodic_receive_timer(self):
        if self.timer:
            self.timer.deinit()
        self.led_pin.off()

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
                    buf += self.nrf.recv().decode().replace("\x00", "")
                except UnicodeError:
                    pass  # Handle partial/invalid packets gracefully
            return buf

    def await_answer(self, expected_answers: list[str], timeout: int, timeout_message: str) -> response:
        self.stop_periodic_receive_timer()
        expected_response_found = False
        response = ""
        start = ticks_ms()
        start_dots = ticks_ms()
        while not expected_response_found:
            for expected_answer in expected_answers:
                if expected_answer in response:
                    expected_response_found = True
            buf = self.receive()
            if buf:
                response += buf
                start = ticks_ms()
            if ticks_diff(ticks_ms(), start) > timeout:
                self.start_periodic_receive_timer()

                raise OSError(errno.ETIMEDOUT, timeout_message+f"\nReceived:\n{response}")
        self.start_periodic_receive_timer()

        return response

    def send(self, string="") -> str:
        self.state = state.TRANSMITTER
        sleep_ms(20)
        self.nrf.stop_listening()
        self.nrf.send_ascii(string + '\n')
        self.nrf.start_listening()
        self.state = state.RECEIVER

        response = self.await_answer(["Command Passed", "Command Failed"], self.DEFAULT_COMMAND_ACK_TIMEOUT, "Didn't receive command acknowledgement from STC Microcontroller!")
        if "Command Failed" in response:
            raise ValueError("Robot Command Failed\n", response)
        return response

    def get_device_time_ms(self) -> str:
        return self.send("T")

    def print_nrf_registers(self) -> str:
        return self.send("N")

    def process(self, exec_dict):
        """
        Process commands from the execution dictionary
        Returns a response dictionary with the same keys as the processed commands
        """
        response_dict = {}
        
        # Iterate through all commands in the execution dictionary
        for key, params in exec_dict.items():
            target = key[0]
            method_name = key[1]
            
            try:
                # Get the target component
                component = getattr(self, target, None)
                if component is None:
                    response_dict[key] = f"Error: Component '{target}' not found"
                    continue
                
                # Get the method to call
                method = getattr(component, method_name, None)
                if method is None:
                    response_dict[key] = f"Error: Method '{method_name}' not found in {target}"
                    continue
                
                # Call the method with parameters if available
                if params:
                    # Convert parameters to appropriate types
                    converted_params = {}
                    for param, value in params.items():
                        try:
                            # Try to convert to integer if possible
                            converted_params[param] = int(value) if value.isdigit() else value
                        except:
                            converted_params[param] = value
                    
                    # Call method with converted parameters
                    response = method(**converted_params)
                else:
                    # Call method without parameters
                    response = method()
                
                # Store the response
                response_dict[key] = response
                
            except Exception as e:
                response_dict[key] = f"Error executing {target}.{method_name}: {str(e)}"
        
        return response_dict

# Controller Components (using composition)
class DifferentialControl:
    def __init__(self, station):
        self.station = station

    def forward(self, distance: int, speed: int = None) -> str:
        cmd = f"Fi{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"

        response = self.station.send(cmd)
        timeout = int(distance*1000/2)  # very generous time to execute
        response += self.station.await_answer(["Differential Control Finished"], timeout, "Didn't Receive Differential Control Finished Movement Acknowledgement")
        return response

    def backward(self, distance: int, speed: int = None) -> str:
        cmd = f"Bi{distance}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"

        response = self.station.send(cmd)
        timeout = int(distance*1000/2)  # very generous time to execute
        response += self.station.await_answer(["Differential Control Finished"], timeout, "Didn't Receive Differential Control Finished Movement Acknowledgement")
        return response

    def right(self, angle: int, speed: int = None) -> str:
        cmd = f"Ri{angle}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"

        response = self.station.send(cmd)
        timeout = int(angle*1000/2)  # very generous time to execute
        response += self.station.await_answer(["Differential Control Finished"], timeout, "Didn't Receive Differential Control Finished Movement Acknowledgement")
        return response

    def left(self, angle: int, speed: int = None) -> str:
        cmd = f"Li{angle}"
        if speed:
            speed_val = int((speed/100)*65535)
            cmd += f"j{speed_val}"

        response = self.station.send(cmd)
        timeout = int(angle*1000/2)  # very generous time to execute
        response += self.station.await_answer(["Differential Control Finished"], timeout, "Didn't Receive Differential Control Finished Movement Acknowledgement")
        return response

class GPS:
    def __init__(self, station):
        self.station = station

    def all(self) -> str:
        return self.station.send("G")

    def latitude(self) -> str:
        return self.station.send("Gi1")

    def longitude(self) -> str:
        return self.station.send("Gi2")

    def heading(self) -> str:
        return self.station.send("Gi3")

    def time(self) -> str:
        return self.station.send("Gi4")

class IMU:
    def __init__(self, station):
        self.station = station

    def orientation_all(self) -> str:
        return self.station.send("M")

    def raw_accel_values(self) -> str:
        return self.station.send("Mi1")

    def raw_gyro_values(self) -> str:
        return self.station.send("Mi2")

    def raw_mag_values(self) -> str:
        return self.station.send("Mi3")

    def accel_offset_values(self) -> str:
        return self.station.send("Mi4")

    def gyro_offset_values(self) -> str:
        return self.station.send("Mi5")

    def mag_offset_values(self) -> str:
        return self.station.send("Mi6")

    def orientation_roll(self) -> str:
        return self.station.send("Mi7")

    def orientation_pitch(self) -> str:
        return self.station.send("Mi8")

    def orientation_yaw(self) -> str:
        return self.station.send("Mi9")

    def unlock_yaw_measurement(self) -> str:
        return self.station.send("Mi10")

    def unlock_yaw_measurement(self) -> str:
        return self.station.send("Mi11")

    def reset_yaw_value(self) -> str:
        return self.station.send("Mi12")

    def mpu6050_internal_registers(self) -> str:
        return self.station.send("Mi13")

    def HMC5883L_internal_registers(self) -> str:
        return self.station.send("Mi14")

class Ultrasonic:
    def __init__(self, station):
        self.station = station

    def stop_measurement(self) -> str:
        return self.station.send("Ui-1")

    def distance_status(self) -> str:
        return self.station.send("Ui0")

    def start_measurement(self) -> str:
        return self.station.send("Ui1")

class ClosedLoopControl:
    CLOSED_LOOP_EXECUTE_MAX_TIMEOUT = 10000
    CLOSED_LOOP_STATUS = ["CLOSED_LOOP_MOVEMENT_IDLE",
                           "CLOSED_LOOP_MOVEMENT_FAILED",
                           "CLOSED_LOOP_MOVEMENT_IN_PROGRESS",
                          "CLOSED_LOOP_MOVEMENT_SUCCESS",
                                 "CL_FAIL_NONE",
                                 "CL_FAIL_MOTOR_ALREADY_MOVING",
                                 "CL_FAIL_MOTOR_RUNAWAY",
                                 "CL_FAIL_MOTOR_WRONG_MOVEMENT",
                                 "CL_FAIL_MOVEMENT_TIMEOUT" 
                                ]
    def __init__(self, station):
        self.station = station

    def reset_idle(self) -> str:
        return self.station.send("Ci-1")

    def status(self) -> str:
        return self.station.send("Ci0")

    def execute_closed_loop_orient(self) -> str:
        response = self.station.send("Ci1j1")
        response += self.station.await_answer(self.CLOSED_LOOP_STATUS, self.CLOSED_LOOP_EXECUTE_MAX_TIMEOUT, f"Closed Loop Control didn't response after {self.CLOSED_LOOP_EXECUTE_MAX_TIMEOUT}ms of sending execution")

        return response

    def execute_closed_loop_move(self) -> str:
        response = self.station.send("Ci1j0")
        response += self.station.await_answer(self.CLOSED_LOOP_STATUS, self.CLOSED_LOOP_EXECUTE_MAX_TIMEOUT, f"Closed Loop Control didn't response after {self.CLOSED_LOOP_EXECUTE_MAX_TIMEOUT}ms of sending execution")

        return response

    def set_setpoint(self, value: int) -> str:
        return self.station.send(f"Ci2j{value}")

    def get_setpoint(self) -> str:
        return self.station.send("Ci3")


class PathPlanning:
    def __init__(self, station):
        self.station = station

    def plan(self, start_loc: int, end_loc: int) -> str:
        return self.station.send(f"Pi{start_loc}j{end_loc}")

    def execute(self) -> str:
        return self.station.send("Ei1")

    def status(self) -> str:
        return self.station.send("Ei0")

    def stop_executing(self) -> str:
        return self.station.send("Ei-1")

