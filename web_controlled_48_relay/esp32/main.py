'''
Main Routine
'''
from wifi import Server
from terminal import Terminal
from relays import Relays
from time import sleep

server = Server()
relays = Relays()

def main():
    server.wait_for_client()
    terminal = Terminal(server, relays)
    try:
        while True:
            terminal(server.client.recv(10).decode())

    finally:
        server.client.close()

# main()
