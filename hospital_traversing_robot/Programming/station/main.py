# main.py
import _thread
import time
import json
from station import Station # Assuming station.py is in the same directory
from server import Server   # Assuming server.py is in the same directory
import machine

# Global dictionaries for communication between threads
# commands_to_execute: { (component_name, method_name): {param1: value1, ...} }
commands_to_execute = {}
# command_responses: { (component_name, method_name): "response string" }
command_responses = {}

# Lock for protecting access to global dictionaries
data_lock = _thread.allocate_lock()

# Station object (will be initialized on core 1 and managed by core 0 for communication)
station_obj = None

def station_core_loop():
    """
    Function to run on the second core (core 1)
    Initializes the Station and continuously processes commands.
    """
    global station_obj, commands_to_execute, command_responses, data_lock

    print("Station Core: Initializing Station...")
    try:
        station_obj = Station()
    except OSError as e:
        print(f"Captured: {e}")
        print("Restarting MCU")
        machine.soft_reset()
    print("Station Core: Station initialized and listening.")

    while True:
        # Check for commands to execute
        with data_lock:
            if commands_to_execute:
                print(f"Station Core: Processing {len(commands_to_execute)} command(s).")
                # Create a copy to process, then clear the original
                current_commands = commands_to_execute.copy()
                commands_to_execute.clear()
            else:
                current_commands = {}

        if current_commands:
            try:
                # Process the commands using the Station's process method
                responses = station_obj.process(current_commands)
                
                with data_lock:
                    command_responses.update(responses)
                print(f"Station Core: Processed commands. Responses: {responses}")
            except Exception as e:
                print(f"Station Core Error during command processing: {e}")
                # Update responses with error for the UI
                with data_lock:
                    for key in current_commands.keys():
                        command_responses[key] = f"Error: {str(e)}"
        
        time.sleep(0.1) # Small delay to prevent busy-waiting

def main():
    """
    Main function to run on the first core (core 0)
    Initializes the web server and starts the second core.
    """
    # time.sleep(3)
    print("Main Core: Starting server initialization...")
    server = Server(commands_to_execute, command_responses, data_lock)
    time.sleep(5)
    
    print("Main Core: Starting station core...")
    _thread.start_new_thread(station_core_loop, ())
    time.sleep(5)

    print("Main Core: Starting web server...")
    server.run()

# main()
