'''
Main Routine
'''
from micropython import const
import machine
from station import Server
from mic import Mic
from dc_motor import DifferencialDrive
from terminal import Terminal
from time import sleep

mic = Mic()
server = Server()
differential_drive = DifferencialDrive()

def main():
    server.wait_for_client()
    terminal = Terminal(server, mic, differential_drive)
    try:
        while True:
            terminal(server.client.recv(10))

    finally:
        server.client.close()

try:
    main()
except Exception as e:
    print(f"Exception: {e}")
    sleep(2)
    machine.reset()

