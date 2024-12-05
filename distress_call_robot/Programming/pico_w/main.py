'''
Main Routine
'''
from micropython import const
import machine
from station import Server
from mic import Mic
from dc_motor import DifferencialDrive
from gps import GPS
from hcsr04 import HCSR04
from pir_sensor import PIRSensor
from terminal import Terminal
from time import sleep

def main():
    server = Server()
    server.wait_for_client()
    terminal = Terminal(server,
                        Mic(Mic.MIC1_ADC_PIN),
                        Mic(Mic.MIC2_ADC_PIN),
                        Mic(Mic.MIC3_ADC_PIN),
                        DifferencialDrive(),
                        GPS(),
                        HCSR04(HCSR04.DEFAULT_TRIGGER_PIN1,
                               HCSR04.DEFAULT_ECHO_PIN1),
                        HCSR04(HCSR04.DEFAULT_TRIGGER_PIN2,
                               HCSR04.DEFAULT_ECHO_PIN2),
                        PIRSensor(PIRSensor.PIN_NUM1),
                        PIRSensor(PIRSensor.PIN_NUM2)
                        )
    try:
        while True:
            terminal(server.client.recv(10))

    except Exception as e:
        print(f"Exception: {e}")
        sleep(2)

    finally:
        server.led.off()
        server.client.close()
        machine.reset()

main()
