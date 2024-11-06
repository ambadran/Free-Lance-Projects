import network
import socket
from time import sleep
from machine import Pin, reset
from micropython import const


ssid = const("Mr.A's Lab")
password = const("lskdmin2938#")

wifi_led = Pin('LED', Pin.OUT)

def connect():
    #Connect to WLAN
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    wlan.connect(ssid, password)
    while wlan.isconnected() == False:
        print('Waiting for connection...')
        sleep(1)
    ip = wlan.ifconfig()[0]
    print(f'Connected on {ip}')
    return ip

def open_socket(ip):
    # Open a socket
    address = (ip, 1234)
    connection = socket.socket()
    connection.bind(address)
    connection.listen(1)

    print(f"Opened socket @ {address}, state: {connection}")

    return connection

def serve(connection):
    #Start a web server
    # state = 'OFF'
    # pico_led.off()
    # temperature = 0
    client = connection.accept()[0]
    while True:
        request = client.recv(1024)
        request = str(request)

        request = request.strip()
        print(request, 'from tcp socket')

        to_be_sent = request + ' from pico\n'
        client.send(to_be_sent)
        # client.close()

def start():
    try:
        ip = connect()
        connection = open_socket(ip)
        serve(connection)
    except KeyboardInterrupt:
        reset()





