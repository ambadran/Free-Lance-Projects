# server.py
from microdot import Microdot, send_file
import time
import json
import uasyncio as asyncio # For async operations if needed later
import network # Import network module

class Server:
    SSID = "Hospital Rover Control"
    PASSWORD = "12345678"

    def __init__(self, commands_to_execute, command_responses, data_lock):
        self.app = Microdot()
        self.commands_to_execute = commands_to_execute
        self.command_responses = command_responses
        self.data_lock = data_lock
        self.app_id = "hospital-rover-control" # A unique identifier for your app

        # Configure WiFi as Access Point
        self.station = network.WLAN(network.AP_IF) # Renamed from 'ap' to 'station' to match user's methods
        self.reset() # Call reset method
        self.init_access_point() # Call init_access_point method

        print('AP IP address:', self.station.ifconfig()[0])

        self._setup_routes()
        print("Server: Microdot app initialized and routes set up.")

    def reset(self):
        """Resets the Access Point interface."""
        self.station.active(False)
        time.sleep(2)
        self.station.active(True)
        print("Server: Access Point reset.")

    def init_access_point(self):
        """Initializes the Access Point configuration."""
        self.station.config(essid=self.SSID, password=self.PASSWORD)
        while not self.station.active():
            print("Server: Access Point Initializing.. ", end=' \r')
            time.sleep(0.5)
        print('Server: Access Point Active!')
        print(f"Server: AP SSID: {self.SSID}, IP: {self.station.ifconfig()[0]}")


    def _setup_routes(self):
        @self.app.route('/')
        async def index(request):
            print("Server: Received request for index.html")
            return send_file('index.html')

        @self.app.route('/command', methods=['POST'])
        async def handle_command(request):
            print(f"Server: Received command request: {request.json}")
            command_data = request.json
            
            target = command_data.get('target')
            method = command_data.get('method')
            params = command_data.get('params', {})

            if not target or not method:
                print("Server Error: Missing target or method in command.")
                return {'status': 'error', 'message': 'Missing target or method'}, 400

            command_key = (target, method)
            
            with self.data_lock:
                self.commands_to_execute[command_key] = params
                # Clear previous response for this command if it exists
                if command_key in self.command_responses:
                    del self.command_responses[command_key]
                print(f"Server: Command '{command_key}' added to execution queue.")
            
            return {'status': 'success', 'message': 'Command queued for execution.'}

        @self.app.route('/status')
        async def get_status(request):
            # This endpoint will return all current responses
            with self.data_lock:
                responses_copy = self.command_responses.copy()
                self.command_responses.clear() # Clear responses after sending to client
            
            # Convert tuple keys to string keys for JSON serialization
            serializable_responses = {str(k): v for k, v in responses_copy.items()}
            
            print(f"Server: Sending status update: {serializable_responses}")
            return json.dumps({'responses': serializable_responses})

    def run(self):
        print("Server: Starting Microdot server...")
        try:
            self.app.run(port=80, debug=True)
        except Exception as e:
            print(f"Server Error: {e}")
        finally:
            self.app.shutdown()
            print("Server: Microdot server shut down.")

