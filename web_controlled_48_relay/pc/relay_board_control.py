'''

'''
import socket
from time import sleep
import re
import scapy.all as scapy
from tkinter.messagebox import showerror
import json

class RelayBoard:
    '''
    Abstraction to control One Relay Board
    '''
    MAX_RELAY_NUM = 16
    DEFAULT_PORT = 1234
    IP_STORAGE_FILE = "relayboard_ips.json"

    COMMAND_PATTERNS = re.compile(r'R(-?\d+)V?(\d?)')

    def __init__(self, board_id):
        self.board_id = board_id
        self.ip_address = self.find_ip_address(board_id)
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        # self.sock.settimeout(1)

        # Establishing connection
        try:
            self.sock.send(b'Connected\n')

        except (BrokenPipeError, OSError):  # MacOS returns this when not connected :P
            self.sock.connect((self.ip_address, self.DEFAULT_PORT))
            print("Connection to 'esp32-relayboard-{self.board_id}' Successful!")
            self.sock.send(b'Connected\n')

        self.relays = []

    def list_ip_addresses(self) -> list[str]:
        '''
        lists all connected devices on wifi network
        '''
        #TODO: import who_is_on_my_wifi library code :D
        pass

    def find_ip_address(self, board_id: int):
        '''
        finds the ip address of a relay board ESP32

        the ESP32's each are named esp32-relayboard-x where x is the board_id
        the name is specifically set as the dhcp hostname. 
        '''
        # Attempt algorithm to find the IP
        # TODO: Implement actual IP-finding logic
        ip_address = None

        # Check manual override in IP storage
        stored_ips = self.load_stored_ips()
        if str(board_id) in stored_ips:
            ip_address = stored_ips[str(board_id)]

        if not ip_address:
            raise ValueError(f"Failed to find IP address for board {board_id}. Configure manually.")

        return ip_address

    @staticmethod
    def load_stored_ips():
        """
        Load stored IP addresses from a file
        """
        try:
            with open(RelayBoard.IP_STORAGE_FILE, "r") as file:
                return json.load(file)
        except FileNotFoundError:
            return {}  # No manual IPs saved yet
        except json.JSONDecodeError:
            showerror("Error", "Failed to load stored IPs. File may be corrupted.")
            return {}

    @staticmethod
    def save_ip_address(board_id: int, ip_address: str):
        """
        Save a manually configured IP address for a board
        """
        stored_ips = RelayBoard.load_stored_ips()
        stored_ips[str(board_id)] = ip_address
        try:
            with open(RelayBoard.IP_STORAGE_FILE, "w") as file:
                json.dump(stored_ips, file, indent=4)
        except Exception as e:
            showerror("Error", f"Failed to save IP address for board {board_id}: {e}")

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

            showerror("Error", f"Failed to save IP address for board {board_id}: {e}")
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


