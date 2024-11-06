import network
import socket
from time import sleep
from machine import Pin, reset
from micropython import const


ssid = const("Mr.A's Lab")
password = const("lskdmin2938#")

wifi_led = Pin('LED', Pin.OUT)

class Server:
    '''
    Station Server
    '''
    SSID = const("Mr.A's Lab")
    PASSWORD = const("lskdmin2938#")

    DEFAULT_PORT = const(3875)

    def __init__(self):
        '''
        connect wifi and initiate server
        '''
        self.wlan = network.WLAN(network.STA_IF)
        if not self.wlan.active():  # to avoid resetting
            self.connect_wifi()
            self.init_socket()

    def connect_wifi(self):
        '''
        connecting to Wifi
        '''
        self.wlan = network.WLAN(network.STA_IF)

        self.wlan.active(False)
        time.sleep(5)
        self.wlan.active(True)

        self.wlan.connect(ssid, password)
        while self.wlan.isconnected() == False:
            print('Waiting for connection...')
            sleep(1)

        self.ip = wlan.ifconfig()[0]
        print(f'Connected on {self.wlan.ifconfig()}')


    def init_socket(self):
        '''
        initiate TCP/IP socket connection
        '''
        self.address = (self.ip, self.DEFAULT_PORT)
        self.connection = socket.socket()
        self.connection.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.connection.bind(self.addr)
        self.connection.listen(1)  # Reduce the backlog to minimize memory usage
        print('Listening on', self.address)


