from station import Station
from server import Server
import time
import _thread
import gc  # For garbage collection to manage memory

# Shared dictionaries
cmd_response_dict = {}
exec_cmd_dict = {}
exec_cmd_lock = _thread.allocate_lock()  # Add a lock for thread safety

def controller_core():
    """
    Main Routine for second core 
    send and receive commands through nrf
    """
    global cmd_response_dict
    global exec_cmd_dict
    global exec_cmd_lock

    station = Station()

    while True:
        # Process commands with thread safety
        with exec_cmd_lock:
            # Create a copy to avoid modifying during iteration
            current_commands = exec_cmd_dict.copy()
            exec_cmd_dict.clear()
        
        # Process commands and get responses
        responses = station.process(current_commands)
        
        # Update response dictionary
        with exec_cmd_lock:
            cmd_response_dict.update(responses)
        
        time.sleep_ms(100)  # Yield to other threads
        gc.collect()  # Manage memory in micropython


def server_core():
    '''
    Main Routine for first core
    Host Web App
    '''
    global cmd_response_dict
    global exec_cmd_dict
    global exec_cmd_lock

    server = Server()
    while True:
        # Update response dict with latest from Station
        with exec_cmd_lock:
            server.cmd_response_dict.update(cmd_response_dict)
            cmd_response_dict.clear()  # Clear after updating
        
        # Run Server
        server.wait_for_client()
        html_request = server.identify_html_request()
        server.handle_html_request(html_request)
        
        # Update cmd execute dict with latest from user
        with exec_cmd_lock:
            exec_cmd_dict.update(server.exec_cmd_dict)
            server.exec_cmd_dict.clear()  # Clear after updating

# Give hardware time to initialize
time.sleep(5)
_thread.start_new_thread(controller_core, ())
server_core()
