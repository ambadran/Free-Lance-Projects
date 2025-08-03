import tkinter as tk
from tkinter import font as tkfont, messagebox
import socket
import json
import os
import threading
import time
from collections import deque
from datetime import datetime

# --- Configuration ---
CONFIG_FILE = 'andon_config_v2.json'
DEFAULT_TITLES = [
    "VISUAL", "BASIS WEIGHT", "TENSILE",
    "AIR PERMIABILITY", "HYDRO PRESSURE", "SURFACTANT"
]
DEFAULT_IP_1 = '192.168.1.100'
DEFAULT_IP_2 = '192.168.1.101'
DEFAULT_PORT = 8888
NUM_RELAYS_PER_BOARD = 20
NUM_RELAYS_PER_COLUMN = 5

# --- GUI Styling ---
COLOR_BG = '#333333'
COLOR_FRAME_BG = '#4F4F4F'
COLOR_TEXT = '#FFFFFF'
COLOR_RED = '#FF4136'
COLOR_YELLOW = '#FFDC00'
COLOR_GREEN = '#2ECC40'
COLOR_BUTTON = '#666666'
COLOR_ARROW_ON = '#0074D9' # Blue for ON arrow
COLOR_STATUS_OFF = '#777777'
UP_ARROW = u"\u25B2"
DOWN_ARROW = u"\u25BC"


class ControlColumn:
    """ Manages a single vertical column in the Andon panel. """
    def __init__(self, parent_frame, app_instance, column_index, initial_title):
        self.parent = parent_frame
        self.app = app_instance
        self.column_index = column_index
        
        # States: 'red', 'yellow', 'green', or 'none' for colors
        self.color_state = 'none'
        # States: 0 for OFF, 1 for ON for arrows
        self.up_arrow_state = 0
        self.down_arrow_state = 0

        self.colors_map = {'red': COLOR_RED, 'yellow': COLOR_YELLOW, 'green': COLOR_GREEN}

        self.frame = tk.Frame(self.parent, bg=COLOR_FRAME_BG, relief=tk.GROOVE, bd=2)
        self.frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.title_var = tk.StringVar(value=initial_title)
        self.title_entry = tk.Entry(
            self.frame, textvariable=self.title_var, bg=COLOR_FRAME_BG, fg=COLOR_TEXT,
            font=('Helvetica', 10, 'bold'), relief=tk.FLAT, justify='center'
        )
        self.title_entry.pack(pady=(5, 5), fill=tk.X, padx=5)

        self.status_display = tk.Label(self.frame, bg=COLOR_STATUS_OFF, height=2, relief=tk.SUNKEN, bd=2)
        self.status_display.pack(fill=tk.X, padx=10, pady=5)

        self.red_button = tk.Button(self.frame, bg=COLOR_RED, command=lambda: self.set_color_state('red'), height=1)
        self.yellow_button = tk.Button(self.frame, bg=COLOR_YELLOW, command=lambda: self.set_color_state('yellow'), height=1)
        self.green_button = tk.Button(self.frame, bg=COLOR_GREEN, command=lambda: self.set_color_state('green'), height=1)
        self.red_button.pack(fill=tk.X, padx=10, pady=(5, 2))
        self.yellow_button.pack(fill=tk.X, padx=10, pady=2)
        self.green_button.pack(fill=tk.X, padx=10, pady=(2, 5))

        arrow_frame = tk.Frame(self.frame, bg=COLOR_FRAME_BG)
        arrow_frame.pack(fill=tk.X, pady=5, padx=10)
        
        self.up_button = tk.Button(arrow_frame, text=UP_ARROW, command=self.toggle_up_arrow, bg=COLOR_BUTTON, fg=COLOR_TEXT, font=('Helvetica', 10, 'bold'))
        self.down_button = tk.Button(arrow_frame, text=DOWN_ARROW, command=self.toggle_down_arrow, bg=COLOR_BUTTON, fg=COLOR_TEXT, font=('Helvetica', 10, 'bold'))
        self.up_button.pack(side=tk.LEFT, fill=tk.X, expand=True, ipady=5)
        self.down_button.pack(side=tk.RIGHT, fill=tk.X, expand=True, ipady=5)
        
        self.all_off_button = tk.Button(self.frame, text="ALL OFF", command=self.turn_all_off, bg=COLOR_BUTTON, fg=COLOR_TEXT)
        self.all_off_button.pack(fill=tk.X, padx=10, pady=10)

        self.all_buttons = [self.red_button, self.yellow_button, self.green_button, self.up_button, self.down_button, self.all_off_button]

    def get_relay_indices(self):
        """ Returns a dict of global relay indices for this column. """
        base = self.column_index * NUM_RELAYS_PER_COLUMN
        return {
            'red': base, 'yellow': base + 1, 'green': base + 2,
            'up': base + 3, 'down': base + 4
        }

    def set_color_state(self, color_name):
        """ Handles clicks on Red, Yellow, Green buttons. """
        if self.app.ui_locked: return
        
        indices = self.get_relay_indices()
        commands = []

        # 1. Prepare color commands
        self.color_state = color_name
        self.status_display.config(bg=self.colors_map[color_name])
        for color, index in indices.items():
            if color in self.colors_map: # only red, yellow, green
                state = 1 if color == color_name else 0
                commands.append((index, state))

        # 2. If Red is pressed, also turn off arrows
        if color_name == 'red':
            if self.up_arrow_state == 1:
                self.up_arrow_state = 0
                self.up_button.config(bg=COLOR_BUTTON)
                commands.append((indices['up'], 0))
            if self.down_arrow_state == 1:
                self.down_arrow_state = 0
                self.down_button.config(bg=COLOR_BUTTON)
                commands.append((indices['down'], 0))
        
        self.app.dispatch_batch(commands)

    def toggle_up_arrow(self):
        if self.app.ui_locked: return
        self.up_arrow_state = 1 - self.up_arrow_state # Toggle 0 and 1
        self.up_button.config(bg=COLOR_ARROW_ON if self.up_arrow_state else COLOR_BUTTON)
        index = self.get_relay_indices()['up']
        self.app.dispatch_command(index, self.up_arrow_state)

    def toggle_down_arrow(self):
        if self.app.ui_locked: return
        self.down_arrow_state = 1 - self.down_arrow_state # Toggle 0 and 1
        self.down_button.config(bg=COLOR_ARROW_ON if self.down_arrow_state else COLOR_BUTTON)
        index = self.get_relay_indices()['down']
        self.app.dispatch_command(index, self.down_arrow_state)

    def turn_all_off(self):
        if self.app.ui_locked: return
        
        # Update GUI immediately
        self.color_state = 'none'
        self.up_arrow_state = 0
        self.down_arrow_state = 0
        self.status_display.config(bg=COLOR_STATUS_OFF)
        self.up_button.config(bg=COLOR_BUTTON)
        self.down_button.config(bg=COLOR_BUTTON)

        # Prepare batch of commands
        indices = self.get_relay_indices()
        commands = [(index, 0) for index in indices.values()]
        self.app.dispatch_batch(commands)

    def get_title(self):
        return self.title_var.get()

    def set_button_state(self, enabled):
        state = tk.NORMAL if enabled else tk.DISABLED
        for btn in self.all_buttons:
            btn.config(state=state)


class AndonApp(tk.Tk):
    """ The main application window. """
    def __init__(self):
        super().__init__()
        self.title("Andon Control Panel v2")
        self.geometry("1100x600")
        self.configure(bg=COLOR_BG)
        self.protocol("WM_DELETE_WINDOW", self.on_closing)

        # Network State
        self.sockets = {1: None, 2: None}
        self.connections = {1: False, 2: False}
        self.ack_queues = {1: deque(), 2: deque()}
        self.listener_threads = {1: None, 2: None}
        
        # UI State
        self.ui_locked = False
        self.commands_pending = 0

        self.control_columns = []
        self._create_widgets()
        self.load_titles()

    def _create_widgets(self):
        title_font = tkfont.Font(family="Helvetica", size=24, weight="bold")
        tk.Label(self, text="Andon Control Panel", bg=COLOR_BG, fg=COLOR_TEXT, font=title_font).pack(pady=(10, 5))

        columns_frame = tk.Frame(self, bg=COLOR_BG)
        columns_frame.pack(fill=tk.BOTH, expand=True, padx=5)

        for i in range(6):
            col = ControlColumn(columns_frame, self, i, DEFAULT_TITLES[i])
            self.control_columns.append(col)
        
        conn_main_frame = tk.Frame(self, bg=COLOR_BG)
        conn_main_frame.pack(side=tk.BOTTOM, fill=tk.X, pady=5)

        # Connection panels for Board 1 and 2
        self.conn_panels = {}
        self._create_conn_panel(conn_main_frame, 1, DEFAULT_IP_1)
        self._create_conn_panel(conn_main_frame, 2, DEFAULT_IP_2)

        self.status_var = tk.StringVar(value="Ready. Please connect to controllers.")
        tk.Label(self, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W, bd=1, bg='#222222', fg=COLOR_TEXT).pack(side=tk.BOTTOM, fill=tk.X)

    def _create_conn_panel(self, parent, board_id, default_ip):
        frame = tk.Frame(parent, bg=COLOR_FRAME_BG, pady=10, relief=tk.GROOVE, bd=1)
        frame.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=5)
        
        tk.Label(frame, text=f"Board {board_id} IP:", bg=COLOR_FRAME_BG, fg=COLOR_TEXT).pack(side=tk.LEFT, padx=(10,0))
        ip_entry = tk.Entry(frame, width=15)
        ip_entry.pack(side=tk.LEFT, padx=5)
        ip_entry.insert(0, default_ip)
        
        tk.Label(frame, text="Port:", bg=COLOR_FRAME_BG, fg=COLOR_TEXT).pack(side=tk.LEFT, padx=5)
        port_entry = tk.Entry(frame, width=7)
        port_entry.pack(side=tk.LEFT, padx=5)
        port_entry.insert(0, str(DEFAULT_PORT))
        
        conn_button = tk.Button(frame, text="Connect", width=12, command=lambda: self.toggle_connection(board_id), bg=COLOR_BUTTON, fg=COLOR_TEXT)
        conn_button.pack(side=tk.LEFT, padx=10)

        self.conn_panels[board_id] = {'ip': ip_entry, 'port': port_entry, 'btn': conn_button}

    def update_status(self, message, temporary=False):
        self.status_var.set(message)
        if temporary:
            self.after(3000, lambda: self.status_var.set("Ready."))

    def set_ui_lock(self, locked, num_commands=0):
        """ Locks or unlocks the entire command UI. """
        self.ui_locked = locked
        state_normal = not locked
        
        if locked:
            self.commands_pending = num_commands
            self.update_status(f"Sending {num_commands} command(s)...")
        else:
            self.commands_pending = 0
            self.update_status("Ready.")

        for col in self.control_columns:
            col.set_button_state(state_normal)

    def acknowledge_command(self, board_id):
        """ Called from listener thread when an ACK is received. """
        if self.commands_pending > 0:
            self.commands_pending -= 1
        
        if self.commands_pending <= 0:
            self.set_ui_lock(False)

    def toggle_connection(self, board_id):
        if self.connections[board_id]:
            self.disconnect_tcp(board_id)
        else:
            panel = self.conn_panels[board_id]
            ip = panel['ip'].get()
            port = int(panel['port'].get())
            threading.Thread(target=self.connect_tcp, args=(board_id, ip, port), daemon=True).start()

    def connect_tcp(self, board_id, ip, port):
        panel = self.conn_panels[board_id]
        try:
            self.update_status(f"Board {board_id}: Connecting to {ip}:{port}...")
            sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            sock.settimeout(5)
            sock.connect((ip, port))
            
            self.sockets[board_id] = sock
            self.connections[board_id] = True
            
            self.listener_threads[board_id] = threading.Thread(target=self.listen_for_acks, args=(board_id,), daemon=True)
            self.listener_threads[board_id].start()

            panel['btn'].config(text="Disconnect", bg=COLOR_RED)
            panel['ip'].config(state='disabled')
            panel['port'].config(state='disabled')
            self.update_status(f"Board {board_id}: Connection successful.")

        except Exception as e:
            self.update_status(f"Board {board_id}: Connection failed: {e}", temporary=True)
            self.sockets[board_id] = None

    def disconnect_tcp(self, board_id):
        self.connections[board_id] = False # Signal listener to stop
        if self.sockets[board_id]:
            try:
                self.sockets[board_id].close()
            except Exception as e:
                print(f"Error closing socket {board_id}: {e}")
        
        self.sockets[board_id] = None
        panel = self.conn_panels[board_id]
        panel['btn'].config(text="Connect", bg=COLOR_BUTTON)
        panel['ip'].config(state='normal')
        panel['port'].config(state='normal')
        self.update_status(f"Board {board_id}: Disconnected.")

    def listen_for_acks(self, board_id):
        """ Listens for 'OK\n' from a controller. Runs in a thread. """
        sock = self.sockets[board_id]
        buffer = ""
        while self.connections[board_id]:
            try:
                sock.settimeout(1.0) # Timeout to allow checking connection flag
                data = sock.recv(64)
                if not data:
                    self.disconnect_tcp(board_id)
                    break
                
                buffer += data.decode('utf-8')
                while '\n' in buffer:
                    ack, buffer = buffer.split('\n', 1)
                    if ack.strip() == "OK":
                        self.acknowledge_command(board_id)

            except socket.timeout:
                continue
            except Exception as e:
                print(f"Listener {board_id} error: {e}")
                self.disconnect_tcp(board_id)
                break
        print(f"Listener thread for board {board_id} finished.")

    def dispatch_command(self, global_index, state):
        """ Convenience method to send a single command. """
        self.dispatch_batch([(global_index, state)])

    def dispatch_batch(self, command_list):
        """ Sends a batch of commands without waiting for individual ACKs. """
        if self.ui_locked:
            self.update_status("Busy, please wait for the current operation to complete.", temporary=True)
            return

        self.set_ui_lock(True, num_commands=len(command_list))

        for global_index, state in command_list:
            # Determine which board and local index to use
            if global_index < NUM_RELAYS_PER_BOARD:
                board_id = 1
                local_index = global_index
            else:
                board_id = 2
                local_index = global_index - NUM_RELAYS_PER_BOARD

            if self.connections[board_id]:
                command = f"{local_index},{state}\n"
                try:
                    self.sockets[board_id].sendall(command.encode('utf-8'))
                    print(f"Sent to Board {board_id}: {command.strip()}")
                except Exception as e:
                    self.update_status(f"Board {board_id} send failed: {e}", temporary=True)
                    self.disconnect_tcp(board_id)
                    self.set_ui_lock(False) # Unlock UI on error
                    return
            else:
                self.update_status(f"Cannot send: Board {board_id} is not connected.", temporary=True)
                self.set_ui_lock(False) # Unlock UI if board is not connected
                return

    def load_titles(self):
        if os.path.exists(CONFIG_FILE):
            try:
                with open(CONFIG_FILE, 'r') as f:
                    titles = json.load(f)
                    if isinstance(titles, list) and len(titles) == 6:
                        for i, col in enumerate(self.control_columns):
                            col.title_var.set(titles[i])
            except Exception as e:
                print(f"Error loading config: {e}")

    def save_titles(self):
        titles = [col.get_title() for col in self.control_columns]
        try:
            with open(CONFIG_FILE, 'w') as f:
                json.dump(titles, f, indent=4)
        except Exception as e:
            self.update_status(f"Error saving titles: {e}")

    def on_closing(self):
        if messagebox.askokcancel("Quit", "Do you want to exit?"):
            self.save_titles()
            self.disconnect_tcp(1)
            self.disconnect_tcp(2)
            self.destroy()


if __name__ == "__main__":
    # --- SET YOUR DESIRED EXPIRATION DATETIME HERE (YEAR, MONTH, DAY, HOUR, MINUTE, SECOND) ---
    EXPIRATION_DATETIME = datetime(2025, 8, 3, 18, 00, 00)
    # Set the check interval in milliseconds (e.g., 1 hour = 3,600,000 ms)
    CHECK_INTERVAL_MS = 1000  # 5min 

    def periodic_expiration_check(app_instance):
        """
        Checks the datetime periodically and shuts down the app if it has expired.
        """
        try:
            if datetime.now() > EXPIRATION_DATETIME:
                # Use after() to ensure this runs in the main GUI thread
                app_instance.after(0, lambda: messagebox.showerror(
                    "Trial Expired",
                    "The trial period has expired. The application will now close."
                ))
                # Use after() to schedule the destruction of the window safely
                app_instance.after(100, app_instance.destroy)
            else:
                # If not expired, schedule the next check
                app_instance.after(CHECK_INTERVAL_MS, lambda: periodic_expiration_check(app_instance))
        except Exception as e:
            print(f"Periodic time check error: {e}")

    # --- INITIAL EXPIRATION CHECK ---
    is_expired = False
    try:
        if datetime.now() > EXPIRATION_DATETIME:
            is_expired = True
    except Exception as e:
        print(f"Initial time check error: {e}")
    
    if is_expired:
        root = tk.Tk()
        root.withdraw()
        messagebox.showerror(
            "Trial Expired",
            "This trial version has expired.\nPlease contact the developer for the full version."
        )
        root.destroy()
    else:
        # If not expired, create and run the main application
        app = AndonApp()
        # Start the first periodic check after the specified interval
        app.after(CHECK_INTERVAL_MS, lambda: periodic_expiration_check(app))
        app.mainloop()
