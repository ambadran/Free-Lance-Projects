# main.py

import socket
import time
from relays import RelayController
from wifi import WiFiManager
from config import configurations
from led import DebugLED # Import the new DebugLED class

# --- Configurations ---
TCP_PORT = configurations["TCP_PORT"]

# --- LED Setup ---
# Instantiate the DebugLED class
led = DebugLED() 
# Use the 'on' method to turn the LED on at boot
led.on()
print("Onboard LED is ON (Solid).")

def run_server():
    """ Initializes all components and starts the TCP server. """
    try:
        relay_controller = RelayController()
    except Exception as e:
        print(f"Failed to initialize RelayController: {e}")
        return

    wifi = WiFiManager() 
    if not wifi.connect():
        print("Halting program. Cannot connect to Wi-Fi.")
        # LED remains solid to indicate connection failure
        return

    # --- Use the new blink method ---
    # Start blinking LED on successful Wi-Fi connection
    print("Wi-Fi connected. Starting LED blink.")
    led.blink()

    ip_address = wifi.get_ip()
    addr = socket.getaddrinfo(ip_address, TCP_PORT)[0][-1]

    s = socket.socket()
    s.bind(addr)
    s.listen(1)
    print(f"🚀 TCP Server is listening on {ip_address}:{TCP_PORT}")

    while True:
        try:
            print("Waiting for a client to connect...")
            conn, addr = s.accept()
            print(f"Client connected from: {addr}")
            
            buffer = ""
            while True:
                data = conn.recv(128)
                if not data:
                    break
                
                buffer += data.decode('utf-8')
                
                while '\n' in buffer:
                    command, buffer = buffer.split('\n', 1)
                    command = command.strip()
                    if not command:
                        continue
                    
                    try:
                        parts = command.split(',')
                        if len(parts) == 2:
                            relay_index = int(parts[0])
                            relay_state = int(parts[1])
                            
                            success = relay_controller.set_relay(relay_index, relay_state)
                            
                            if success:
                                conn.sendall(b'OK\n')

                    except (ValueError, IndexError) as e:
                        print(f"Received malformed command: '{command}'. Error: {e}")

            conn.close()
            print("Client disconnected.")

        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            # On crash, stop blinking and set LED to solid to indicate an error
            led.stop_blink()
            led.on()
            
            if 'conn' in locals() and conn:
                conn.close()
            time.sleep(5)

# time.sleep(3)
# run_server()
