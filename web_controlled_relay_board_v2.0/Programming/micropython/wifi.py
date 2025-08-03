# wifi.py

import network
import time
import sys
from config import configurations

class WiFiManager:
    """
    Manages the Wi-Fi connection for different MicroPython boards (Pico W, ESP32).
    It automatically uses credentials from the 'config.py' file.
    """
    SSID = configurations.get("WIFI_SSID")
    PASSWORD = configurations.get("WIFI_PASSWORD")

    def __init__(self, ssid: str = SSID, password: str = PASSWORD):
        """
        Initializes the Wi-Fi manager.
        """
        if not ssid or not password:
            raise ValueError("WIFI_SSID and WIFI_PASSWORD must be defined in configurations.")
            
        self.ssid = ssid
        self.password = password
        self.wlan = network.WLAN(network.STA_IF)
        self.ip_address = None
        
        # Print the detected platform for debugging purposes
        print(f"Initializing Wi-Fi for platform: {sys.platform}")

    def connect(self) -> bool:
        """
        Activates the WLAN interface and connects to the specified network.
        This method handles platform-specific timings.

        Returns:
            bool: True if connection is successful, False otherwise.
        """
        if self.wlan.isconnected():
            print("Already connected to Wi-Fi.")
            self.ip_address = self.wlan.ifconfig()[0]
            return True

        # Activate the network interface
        self.wlan.active(True)

        # Some ESP32 boards benefit from a small delay after activation
        if sys.platform == 'esp32':
            time.sleep_ms(500)

        # Initiate connection
        self.wlan.connect(self.ssid, self.password)

        print(f"Attempting to connect to Wi-Fi network: {self.ssid}...")
        
        # Wait for connection with a 15-second timeout
        max_wait = 15
        while max_wait > 0:
            # isconnected() is a reliable check for both platforms
            if self.wlan.isconnected():
                break
            max_wait -= 1
            print(".", end="")
            time.sleep(1)

        # Check final connection status
        if not self.wlan.isconnected():
            print("\n❌ Wi-Fi connection failed!")
            self.wlan.active(False) # Turn off Wi-Fi to save power
            return False
        else:
            self.ip_address = self.wlan.ifconfig()[0]
            print(f"\n✅ Wi-Fi connected! IP Address: {self.ip_address}")
            return True
            
    def get_ip(self) -> str:
        """Returns the current IP address."""
        return self.ip_address

