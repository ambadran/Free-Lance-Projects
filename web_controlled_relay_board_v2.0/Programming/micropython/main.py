# main.py

import socket
import time
from machine import Pin, Timer
from relays import RelayController
from wifi import WiFiManager

# --- Configuration ---
# UPDATE THESE WITH YOUR WIFI DETAILS
WIFI_SSID = "Mr.A's iPhone"
WIFI_PASSWORD = "bdkdinvg"
TCP_PORT = 8888  # Must match the port used in your PC's Python GUI

# --- LED Setup ---
# Initialize the onboard LED pin as an output
led = Pin('LED', Pin.OUT)
# Turn the LED on immediately upon script execution
led.on()
print("Onboard LED is ON (Solid).")

def run_server():
    """
    Initializes all components and starts the TCP server.
    """
    # 1. Initialize Relay Controller
    # This will set up pins GP0-GP19 and turn them all off.
    try:
        relay_controller = RelayController(num_relays=20)
    except Exception as e:
        print(f"Failed to initialize RelayController: {e}")
        return

    # 2. Connect to Wi-Fi
    wifi = WiFiManager(WIFI_SSID, WIFI_PASSWORD)
    if not wifi.connect():
        print("Halting program. Cannot connect to Wi-Fi.")
        return # Stop execution if Wi-Fi fails

    print("Wi-Fi connected. Starting LED blink.")
    blink_timer = Timer()
    blink_timer.init(freq=2.0, mode=Timer.PERIODIC, callback=lambda t: led.toggle())

    # 3. Set up TCP Server Socket
    ip_address = wifi.get_ip()
    addr = socket.getaddrinfo(ip_address, TCP_PORT)[0][-1]

    s = socket.socket()
    s.bind(addr)
    s.listen(1) # Listen for one client at a time

    print(f"TCP Server is listening on {ip_address}:{TCP_PORT}")

    # 4. Main loop to accept connections
    while True:
        try:
            print("Waiting for a client to connect...")
            conn, addr = s.accept()
            print(f"Client connected from: {addr}")

            buffer = "" # Create a buffer to store incoming data
            
            # Loop to handle communication with the connected client
            while True:
                data = conn.recv(128) # Receive up to 128 bytes
                if not data:
                    break # Client disconnected
                
                # Add newly received bytes to the buffer
                buffer += data.decode('utf-8')
                
                # Process the buffer until no complete commands ('\n') are left
                while '\n' in buffer:
                    # Split at the first newline to get one complete command
                    command, buffer = buffer.split('\n', 1)
                    
                    command = command.strip() # Clean up whitespace
                    if not command:
                        continue # Skip if the line is empty
                    
                    # Process the single, complete command
                    try:
                        parts = command.split(',')
                        if len(parts) == 2:
                            relay_index = int(parts[0])
                            relay_state = int(parts[1])
                            relay_controller.set_relay(relay_index, relay_state)
                    except (ValueError, IndexError) as e:
                        print(f"Received malformed command: '{command}'. Error: {e}")
            
            conn.close()
            print("Client disconnected.")

        except OSError as e:
            # Handle potential socket errors
            print(f"A server error occurred: {e}")
            if conn:
                conn.close()
            time.sleep(5)
            print("Restarting server listen loop.")
        except Exception as e:
            print(f"An unexpected error occurred: {e}")
            break # Exit the main loop on critical errors
    
    # Cleanup
    s.close()
    print("Server shut down.")

# --- Start the Application ---
time.sleep(3)
run_server()
