# wifi.py

import network
import time

class WiFiManager:
    """
    Manages the Wi-Fi connection for the Raspberry Pi Pico W.
    """
    def __init__(self, ssid: str, password: str):
        """
        Initializes the Wi-Fi manager.

        Args:
            ssid (str): The SSID (name) of the Wi-Fi network.
            password (str): The password for the Wi-Fi network.
        """
        self.ssid = ssid
        self.password = password
        self.wlan = network.WLAN(network.STA_IF)
        self.ip_address = None

    def connect(self) -> bool:
        """
        Activates the WLAN interface and connects to the specified network.

        Returns:
            bool: True if connection is successful, False otherwise.
        """
        if self.wlan.isconnected():
            print("Already connected to Wi-Fi.")
            self.ip_address = self.wlan.ifconfig()[0]
            return True

        self.wlan.active(True)
        self.wlan.connect(self.ssid, self.password)

        print(f"Attempting to connect to Wi-Fi network: {self.ssid}...")
        
        # Wait for connection with a 15-second timeout
        max_wait = 15
        while max_wait > 0:
            status = self.wlan.status()
            if status < 0 or status >= 3:
                break
            max_wait -= 1
            print(".")
            time.sleep(1)

        # Check connection status
        if self.wlan.status() != 3:
            print("❌ Wi-Fi connection failed!")
            self.wlan.active(False) # Turn off Wi-Fi to save power
            return False
        else:
            self.ip_address = self.wlan.ifconfig()[0]
            print(f"✅ Wi-Fi connected! Pico IP Address: {self.ip_address}")
            return True
            
    def get_ip(self) -> str:
        """Returns the current IP address."""
        return self.ip_address
