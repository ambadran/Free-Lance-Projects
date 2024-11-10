'''
Main Routine
'''
from micropython import const
from station import Server
from mic import Mic
from time import sleep

mic = Mic()
server = Server()

def main():
    server.wait_for_client()
    try:
        while True:
            server.client.send(mic.sample())
            sleep(1/mic.sampling_rate)

    finally:
        mic.counter = 0
        server.client.close()

# main()
