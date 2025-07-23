import tkinter as tk
from tkinter import font as tkfont, messagebox
import socket
import json
import os
import threading

# --- Configuration ---
# File to store the custom titles for each column
CONFIG_FILE = 'andon_config.json'

# Default values if no config file is found
DEFAULT_TITLES = [
    "VISUAL", "BASIS WEIGHT", "TENSILE",
    "AIR PERMIABILITY", "HYDRO PRESSURE", "SURFACTANT"
]
DEFAULT_IP = '192.168.1.100'
DEFAULT_PORT = 8888

# --- GUI Styling ---
COLOR_BG = '#333333'
COLOR_FRAME_BG = '#4F4F4F'
COLOR_TEXT = '#FFFFFF'
COLOR_RED = '#FF4136'
COLOR_YELLOW = '#FFDC00'
COLOR_GREEN = '#2ECC40'
COLOR_BUTTON = '#666666'
COLOR_STATUS_OFF = '#777777'

# --- Unicode Arrows ---
UP_ARROW = u"\u25B2"
DOWN_ARROW = u"\u25BC"


class ControlColumn:
    """
    Manages a single vertical column in the Andon panel, 
    including its title, status display, and control buttons.
    """
    def __init__(self, parent_frame, app_instance, column_index, initial_title):
        self.parent = parent_frame
        self.app = app_instance
        self.column_index = column_index
        
        # The order of states for cycling with arrow buttons
        self.state_order = ['red', 'yellow', 'green']
        self.colors_map = {'red': COLOR_RED, 'yellow': COLOR_YELLOW, 'green': COLOR_GREEN}
        self.current_state = 'none' # Can be 'red', 'yellow', 'green', or 'none'

        # Main frame for this column
        self.frame = tk.Frame(self.parent, bg=COLOR_FRAME_BG, relief=tk.GROOVE, bd=2)
        self.frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=10)

        # --- Widgets ---
        self.title_var = tk.StringVar(value=initial_title)
        
        # 1. Editable Title
        self.title_entry = tk.Entry(
            self.frame, 
            textvariable=self.title_var,
            bg=COLOR_FRAME_BG,
            fg=COLOR_TEXT,
            font=('Helvetica', 10, 'bold'),
            relief=tk.FLAT,
            justify='center'
        )
        self.title_entry.pack(pady=(5, 10), fill=tk.X, padx=5)

        # 2. Status Display Box
        self.status_display = tk.Label(
            self.frame, 
            bg=COLOR_STATUS_OFF, 
            height=3, 
            relief=tk.SUNKEN, 
            bd=2
        )
        self.status_display.pack(fill=tk.X, padx=10, pady=5)

        # 3. Color Buttons (Red, Yellow, Green)
        button_font = ('Helvetica', 10, 'bold')
        self.red_button = tk.Button(
            self.frame, bg=COLOR_RED, command=lambda: self.set_state('red'), 
            height=2, relief=tk.GROOVE
        )
        self.yellow_button = tk.Button(
            self.frame, bg=COLOR_YELLOW, command=lambda: self.set_state('yellow'), 
            height=2, relief=tk.GROOVE
        )
        self.green_button = tk.Button(
            self.frame, bg=COLOR_GREEN, command=lambda: self.set_state('green'), 
            height=2, relief=tk.GROOVE
        )
        self.red_button.pack(fill=tk.X, padx=10, pady=(10, 2))
        self.yellow_button.pack(fill=tk.X, padx=10, pady=2)
        self.green_button.pack(fill=tk.X, padx=10, pady=(2, 10))
        
        # 4. Arrow Buttons
        arrow_frame = tk.Frame(self.frame, bg=COLOR_FRAME_BG)
        arrow_frame.pack(fill=tk.X, pady=5, padx=10)
        
        self.up_button = tk.Button(
            arrow_frame, text=UP_ARROW, command=self.cycle_up,
            bg=COLOR_BUTTON, fg=COLOR_TEXT, relief=tk.GROOVE, font=button_font
        )
        self.down_button = tk.Button(
            arrow_frame, text=DOWN_ARROW, command=self.cycle_down,
            bg=COLOR_BUTTON, fg=COLOR_TEXT, relief=tk.GROOVE, font=button_font
        )
        self.up_button.pack(side=tk.LEFT, fill=tk.X, expand=True)
        self.down_button.pack(side=tk.RIGHT, fill=tk.X, expand=True)

    def set_state(self, color_name):
        """ The core logic to set the state, update GUI, and send commands. """
        if not self.app.is_connected:
            self.app.update_status("Error: Not connected to the controller.")
            return

        # Update GUI
        self.current_state = color_name
        self.status_display.config(bg=self.colors_map[color_name])

        # Send TCP commands for all three relays in this column
        # This enforces the "only one ON at a time" rule.
        for i, color in enumerate(self.state_order):
            relay_index = self.column_index * 3 + i
            state = 1 if color == color_name else 0  # 1 for ON, 0 for OFF
            self.app.send_command(relay_index, state)

    def cycle_up(self):
        """Cycles to the next color: Green -> Yellow -> Red -> Green."""
        if self.current_state == 'none':
            next_state = self.state_order[-1] # Start with green
        else:
            current_index = self.state_order.index(self.current_state)
            next_index = (current_index - 1) % len(self.state_order)
            next_state = self.state_order[next_index]
        self.set_state(next_state)

    def cycle_down(self):
        """Cycles to the previous color: Red -> Yellow -> Green -> Red."""
        if self.current_state == 'none':
            next_state = self.state_order[0] # Start with red
        else:
            current_index = self.state_order.index(self.current_state)
            next_index = (current_index + 1) % len(self.state_order)
            next_state = self.state_order[next_index]
        self.set_state(next_state)

    def get_title(self):
        """Returns the current text from the title entry."""
        return self.title_var.get()


class AndonApp(tk.Tk):
    """
    The main application window. Manages the overall layout,
    TCP connection, and configuration persistence.
    """
    def __init__(self):
        super().__init__()
        self.title("Andon Control Panel")
        self.geometry("1000x520")
        self.configure(bg=COLOR_BG)
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        # --- Network State ---
        self.client_socket = None
        self.is_connected = False
        
        self.control_columns = []
        self._create_widgets()
        self.load_titles()

    def _create_widgets(self):
        # Main Title Label
        title_font = tkfont.Font(family="Helvetica", size=24, weight="bold")
        title_label = tk.Label(
            self, text="Andon Control Panel", bg=COLOR_BG, fg=COLOR_TEXT, font=title_font
        )
        title_label.pack(pady=(10, 5))

        # Frame for all ControlColumn instances
        columns_frame = tk.Frame(self, bg=COLOR_BG)
        columns_frame.pack(fill=tk.BOTH, expand=True, padx=5)

        # Create the 6 control columns
        for i in range(6):
            # Pass a default title; it will be updated by load_titles()
            col = ControlColumn(columns_frame, self, i, DEFAULT_TITLES[i])
            self.control_columns.append(col)
        
        # Connection Control Panel at the bottom
        conn_frame = tk.Frame(self, bg=COLOR_FRAME_BG, pady=10)
        conn_frame.pack(side=tk.BOTTOM, fill=tk.X)
        
        tk.Label(conn_frame, text="IP:", bg=COLOR_FRAME_BG, fg=COLOR_TEXT).pack(side=tk.LEFT, padx=(10,0))
        self.ip_entry = tk.Entry(conn_frame, width=15)
        self.ip_entry.pack(side=tk.LEFT, padx=5)
        self.ip_entry.insert(0, DEFAULT_IP)
        
        tk.Label(conn_frame, text="Port:", bg=COLOR_FRAME_BG, fg=COLOR_TEXT).pack(side=tk.LEFT, padx=5)
        self.port_entry = tk.Entry(conn_frame, width=7)
        self.port_entry.pack(side=tk.LEFT, padx=5)
        self.port_entry.insert(0, str(DEFAULT_PORT))
        
        self.conn_button = tk.Button(conn_frame, text="Connect", width=12, command=self.toggle_connection, bg=COLOR_BUTTON, fg=COLOR_TEXT)
        self.conn_button.pack(side=tk.LEFT, padx=10)

        # Status Bar
        self.status_var = tk.StringVar(value="Ready. Please connect to the controller.")
        status_bar = tk.Label(self, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W, bd=1, bg='#222222', fg=COLOR_TEXT)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def update_status(self, message):
        """Updates the text in the status bar."""
        self.status_var.set(message)
        self.update_idletasks()

    def toggle_connection(self):
        """Connects or disconnects the TCP client."""
        if self.is_connected:
            self.disconnect_tcp()
        else:
            ip = self.ip_entry.get()
            port_str = self.port_entry.get()
            if not port_str.isdigit():
                self.update_status("Error: Port must be a number.")
                return
            port = int(port_str)
            # Run connection attempt in a separate thread to avoid freezing the GUI
            threading.Thread(target=self.connect_tcp, args=(ip, port), daemon=True).start()

    def connect_tcp(self, ip, port):
        """Establishes a TCP socket connection."""
        self.update_status(f"Connecting to {ip}:{port}...")
        try:
            self.client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.client_socket.settimeout(5) # 5-second timeout for connection
            self.client_socket.connect((ip, port))
            self.is_connected = True
            self.update_status(f"Successfully connected to {ip}:{port}")
            self.conn_button.config(text="Disconnect", bg=COLOR_RED)
            self.ip_entry.config(state='disabled')
            self.port_entry.config(state='disabled')
        except (socket.timeout, socket.error) as e:
            self.is_connected = False
            self.update_status(f"Connection failed: {e}")
            self.client_socket = None

    def disconnect_tcp(self):
        """Closes the TCP socket connection."""
        if self.client_socket:
            try:
                self.client_socket.close()
            except socket.error as e:
                print(f"Error closing socket: {e}") # Log to console
        self.client_socket = None
        self.is_connected = False
        self.update_status("Disconnected. Ready to connect.")
        self.conn_button.config(text="Connect", bg=COLOR_BUTTON)
        self.ip_entry.config(state='normal')
        self.port_entry.config(state='normal')

    def send_command(self, relay_index, state):
        """Formats and sends a command string over the TCP socket."""
        if self.is_connected and self.client_socket:
            command = f"{relay_index},{state}\n" # e.g., "4,1\n" for Relay 4 ON
            try:
                self.client_socket.sendall(command.encode('utf-8'))
                print(f"Sent: {command.strip()}") # For debugging
            except socket.error as e:
                self.update_status(f"Send failed: {e}. Disconnecting.")
                self.disconnect_tcp()

    def load_titles(self):
        """Loads column titles from the config file."""
        if os.path.exists(CONFIG_FILE):
            try:
                with open(CONFIG_FILE, 'r') as f:
                    titles = json.load(f)
                    if isinstance(titles, list) and len(titles) == 6:
                        for i, col in enumerate(self.control_columns):
                            col.title_var.set(titles[i])
                        return
            except (json.JSONDecodeError, TypeError):
                self.update_status(f"Warning: Could not read '{CONFIG_FILE}'. Using defaults.")
        # Use defaults if file doesn't exist or is invalid
        for i, col in enumerate(self.control_columns):
            col.title_var.set(DEFAULT_TITLES[i])

    def save_titles(self):
        """Saves the current column titles to the config file."""
        titles = [col.get_title() for col in self.control_columns]
        try:
            with open(CONFIG_FILE, 'w') as f:
                json.dump(titles, f, indent=4)
        except IOError as e:
            self.update_status(f"Error saving titles: {e}")

    def on_closing(self):
        """Handles the window closing event."""
        if messagebox.askokcancel("Quit", "Do you want to exit?"):
            self.save_titles()
            self.disconnect_tcp()
            self.destroy()

if __name__ == "__main__":
    app = AndonApp()
    app.mainloop()
