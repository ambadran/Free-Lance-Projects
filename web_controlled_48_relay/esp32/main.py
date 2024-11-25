'''
Main Routine
'''
from wifi import Server
from terminal import Terminal
from relays import Relays
from time import sleep
import machine

server = Server()
relays = Relays()

def main():
    server.wait_for_client()
    terminal = Terminal(server, relays)
    while True:
        terminal(server.client.recv(10))


try:
    main()
except Exception as e:
    print("Caught Exception: {e}")
    server.client.close()
    machine.reset()
