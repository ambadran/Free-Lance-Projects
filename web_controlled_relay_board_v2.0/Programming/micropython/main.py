# main.py

import socket
import time
from relays import RelayController
from wifi import WiFiManager

# --- Configuration ---
# UPDATE THESE WITH YOUR WIFI DETAILS
WIFI_SSID = "Mr.A's Lab"
WIFI_PASSWORD = "aslkdjf234"
TCP_PORT = 8888  # Must match the port used in your PC's Python GUI

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

    # 3. Set up TCP Server Socket
    ip_address = wifi.get_ip()
    addr = socket.getaddrinfo(ip_address, TCP_PORT)[0][-1]

    s = socket.socket()
    s.bind(addr)
    s.listen(1) # Listen for one client at a time

    print(f"🚀 TCP Server is listening on {ip_address}:{TCP_PORT}")

    # 4. Main loop to accept connections
    while True:
        try:
            print("Waiting for a client to connect...")
            conn, addr = s.accept()
            print(f"Client connected from: {addr}")

            # Loop to handle communication with the connected client
            while True:
                data = conn.recv(128) # Receive up to 128 bytes
                if not data:
                    break # Client disconnected
                
                # Process the received command
                try:
                    # Expected format from GUI: "index,state\n"
                    command = data.decode('utf-8').strip()
                    parts = command.split(',')
                    if len(parts) == 2:
                        relay_index = int(parts[0])
                        relay_state = int(parts[1])
                        relay_controller.set_relay(relay_index, relay_state)
                except (ValueError, IndexError) as e:
                    print(f"Received malformed data: '{data.decode('utf-8')}'. Error: {e}")

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
# if __name__ == "__main__":
#     run_server()
