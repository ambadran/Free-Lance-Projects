'''
Main Routine
'''
from micropython import const
import machine
from station import Server
from mic import Mic
from terminal import Terminal
from time import sleep

mic = Mic()
server = Server()

def main():
    server.wait_for_client()
    terminal = Terminal(server, mic)
    try:
        while True:
            terminal(server.client.recv(10).decode())

    finally:
        server.client.close()

# main()
