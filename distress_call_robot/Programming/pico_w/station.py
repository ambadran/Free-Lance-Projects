'''


'''
from micropython import const
from machine import Pin
import network
import socket
import json
from time import sleep, sleep_ms, ticks_ms, ticks_diff
import random

class Server:
    # Access Point Parameters
    SSID = "WE_DE3960"
    PASSWORD = "01003889419$"
    DEFAULT_PORT = 1234

    def __init__(self):
        '''
        initiate server
        '''
        self.led = Pin("LED", Pin.OUT)  # on-board LED to show state
        self.led.off()

        self.reset()
        #TODO: implement try except block to avoid redefining socket
        self.init_wifi_connection()
        self.init_socket()
        self.led.on()

    def reset(self):
        '''
        returns station object on reset.
        just deactivate and activate again 
        '''
        self.station = network.WLAN(network.STA_IF)

        self.station.active(False)
        sleep(2)
        self.station.active(True)

        self.station.connect(self.SSID, self.PASSWORD)

    def init_wifi_connection(self):
        '''
        set up the Access Point
        '''
        self.station.connect(self.SSID, self.PASSWORD)

        while self.station.isconnected() == False:
            print(f"Connecting to WiFi.. ", end=' \r')

        self.ip = self.station.ifconfig()[0]

        print('\nConnected to Wifi!!\n')
        print(self.station.ifconfig())

    def init_socket(self):
        '''
        initiate socket connection
        '''
        try:
            # self.addr = socket.getaddrinfo('0.0.0.0', 80)[0][-1]
            self.addr = (self.ip, self.DEFAULT_PORT)
            self.s = socket.socket()
            self.s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
            self.s.bind(self.addr)
            self.s.listen(1)  # Reduce the backlog to minimize memory usage
            print(f'Listening on: {self.addr}')
            print(f'Connection: {self.s}')

            return self.s

        except OSError as e:
            print(f"Caught: {e}")

    def wait_for_client(self):
        '''
        Await client to connect then return new socket object used to 
        communicate with the connected client. 
        This socket is distinct from the listening socket (s) 
        and is used for sending and receiving data with the specific client that connected.
        '''
        # try:
        self.client, addr = self.s.accept()
        # print('Got a connection from {str(addr)}', end=' \r')
        print(f"Got a connection from {str(addr)}")
        # except Exception as e:
        #     print(f"Caught: {e}")

    def send_audio(self, samples: bytearray):
        '''
        sends start signal
        then sends the bytearray 
        then sends end signal
        '''
        print("Sending audio sample..")
        self.client.send("Start\n\n")

        s_time = ticks_ms()
        self.client.sendall(samples)
        e_time = ticks_diff(ticks_ms(), s_time)

        self.client.send("\n\nEnd\n")
        print(f"Sent samples in {e_time}ms!\n")

    def echo_session(self):
        '''
        echos in and out any character, meant for testing purposes
        '''
        try:
            while True:
                self.request = self.client.recv(1024).decode().strip()

                print(self.request, 'from tcp socket')

                to_be_sent = self.request + ' from pico\n'
                self.client.send(to_be_sent)

        except Exception as e:
            print(f"Caught Exception: {e}")
            self.client.close()

def test():
    global server
    server = Server()
    server.wait_for_client()
    server.echo_session()



