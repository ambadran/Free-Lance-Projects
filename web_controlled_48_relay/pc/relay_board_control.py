'''

'''
import socket
from time import sleep
import re
import scapy.all as scapy

class RelayBoard:
    '''
    Abstraction to control One Relay Board
    '''
    MAX_RELAY_NUM = 16
    DEFAULT_PORT = 1234

    COMMAND_PATTERNS = re.compile(r'R(-?\d+)V?(\d?)')

    # BOARD_ID_IP_ADD = {1: '192.168.1.10', 2: '192.168.1.11', 3: '192.168.1.12'}
    BOARD_ID_IP_ADD = {1: '192.168.1.10', 2: '192.168.1.11', 3: '172.20.10.5'}

    def __init__(self, board_id):
        self.board_id = board_id
        self.ip_address = self.find_ip_address(board_id)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.sock.settimeout(1)

        # Establishing connection
        try:
            self.sock.send(b'\n')

        except (BrokenPipeError, OSError):  # MacOS returns this when not connected :P
            self.sock.connect((self.ip_address, self.DEFAULT_PORT))
            print("Connection to 'esp32-relayboard-{self.board_id}' Successful!")

            self.sock.send(b'\n')

        self.relays = []

    def list_ip_addresses(self) -> list[str]:
        '''
        lists all connected devices on wifi network
        '''
        #TODO: import who_is_on_my_wifi library code :D

    def find_ip_address(self, board_id: int):
        '''
        finds the ip address of a relay board ESP32

        the ESP32's each are named esp32-relayboard-x where x is the board_id
        the name is specifically set as the dhcp hostname. 
        '''
        #TODO: implement someway to read dhcp hostname
        return self.BOARD_ID_IP_ADD[board_id]

    @classmethod
    def decode_cmd(cls, cmd) -> int | None:
        '''
        returns the value of given command
        '''
        cmd = cmd.strip()
        matched = self.COMMAND_PATTERNS.match(cmd)
        return cmd.groups()[2]

    def value(self, pin_num: int, state: int=None) -> int:
        '''
        Main function to set and read the value of a specific relay
        '''
        if state is None:
            self.sock.send(f'R{pin_num}\n'.encode())  # sending read command to esp32 
            value = self.sock.recv(20).decode().strip()  # receiving value of relay
            return value #TODO: re decode it

        else:
            self.sock.send(f"R{pin_num}V{state}".encode())  # sending set command to esp32
            value = self.sock.recv(20).decode().strip()  # receiving confirmation of relay value
            #TODO: make sure received is same as set
            return value #TODO: re decode it

    def values(self) -> list[int]:
        '''
        Reads values of ALL relays
        '''
        self.sock.send("R-1".encode())
        values = self.sock.recv(100).decode().strip()
        print(values)

    def off(self):
        '''
        turns all relays off
        '''
        self.sock.send("R-1V0".encode())
        values = self.sock.recv(100).decode().strip()
        print(values)


