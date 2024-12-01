'''
Main Routine
'''
from micropython import const
import machine
from station import Server
from mic import Mic
from dc_motor import DifferencialDrive
from gps import GPS
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
                        GPS())
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
