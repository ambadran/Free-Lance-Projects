import tkinter as tk
from tkinter import ttk, messagebox
import socket
import threading
import json
import os
import time # Added for potential delays if needed for debugging

# --- Configuration ---
CONFIG_FILE = "gui_config.json"
HOST = '192.168.1.100'  # Replace with your microcontroller's IP address
PORT = 8888             # Replace with your microcontroller's port

# --- Global Variables ---
client_socket = None
is_connected = False
connection_thread = None # To keep track of the connection thread

# --- Helper Functions ---

def load_config():
    """Loads configuration from a JSON file."""
    if os.path.exists(CONFIG_FILE):
        with open(CONFIG_FILE, 'r') as f:
            return json.load(f)
    return {}

def save_config(config):
    """Saves configuration to a JSON file."""
    with open(CONFIG_FILE, 'w') as f:
        json.dump(config, f, indent=4)

def send_relay_command(column_index, color_index, state):
    """Sends a relay command over TCP."""
    global client_socket, is_connected
    if not is_connected or client_socket is None:
        messagebox.showwarning("Connection Error", "Not connected to the microcontroller.")
        return

    command = f"{column_index},{color_index},{state}\n" # Added newline for simpler reading on microcontroller side
    try:
        client_socket.sendall(command.encode('utf-8'))
        print(f"Sent command: {command.strip()}")
    except BrokenPipeError:
        # Schedule the disconnection and error message in the main thread
        root.after(0, lambda: messagebox.showerror("Connection Error", "Microcontroller disconnected unexpectedly."))
        root.after(0, disconnect_tcp)
    except Exception as e:
        root.after(0, lambda: messagebox.showerror("Communication Error", f"Failed to send command: {e}"))

def update_gui_status(text, fg_color):
    """Safely updates the status label from any thread."""
    root.after(0, lambda: status_label.config(text=text, fg=fg_color))

def show_message_box_thread_safe(title, message, message_type="info"):
    """Safely displays a messagebox from any thread."""
    if message_type == "info":
        root.after(0, lambda: messagebox.showinfo(title, message))
    elif message_type == "error":
        root.after(0, lambda: messagebox.showerror(title, message))
    elif message_type == "warning":
        root.after(0, lambda: messagebox.showwarning(title, message))

def set_button_states(state):
    """Enables/disables all relay control buttons."""
    for col_idx in range(len(columns)):
        for btn_idx in range(3):
            color_buttons[col_idx][btn_idx].config(state=state)
        up_buttons[col_idx].config(state=state)
        down_buttons[col_idx].config(state=state)

def connect_tcp():
    """Attempts to connect to the TCP server in a separate thread."""
    global client_socket, is_connected, connection_thread
    if is_connected:
        show_message_box_thread_safe("Connection Status", "Already connected.")
        return

    if connection_thread and connection_thread.is_alive():
        update_gui_status("Connection in progress...", "orange")
        return # Don't start another connection attempt if one is already running

    def _connect():
        global client_socket, is_connected
        try:
            update_gui_status("Connecting...", "orange")
            root.update_idletasks() # Update GUI immediately

            client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            client_socket.settimeout(10) # Increased timeout for connection (e.g., 10 seconds)
            client_socket.connect((HOST, PORT))
            is_connected = True

            root.after(0, lambda: toggle_connection_button.config(text="Disconnect"))
            update_gui_status(f"Connected to {HOST}:{PORT}", "green")
            show_message_box_thread_safe("Connection", f"Successfully connected to {HOST}:{PORT}")
            root.after(0, lambda: set_button_states(tk.NORMAL)) # Enable buttons

        except (socket.timeout, ConnectionRefusedError, socket.gaierror) as e:
            is_connected = False
            client_socket = None
            error_message = ""
            if isinstance(e, socket.timeout):
                error_message = "Connection attempt timed out. Check IP/Port or microcontroller."
            elif isinstance(e, ConnectionRefusedError):
                error_message = "Connection refused. Make sure microcontroller is running."
            elif isinstance(e, socket.gaierror):
                error_message = f"Address error: {e}. Check IP address."
            else:
                error_message = f"Connection error: {e}" # Fallback for other socket errors

            update_gui_status(error_message, "red")
            show_message_box_thread_safe("Connection Error", error_message, "error")
            root.after(0, lambda: set_button_states(tk.DISABLED)) # Ensure buttons are disabled

        except Exception as e:
            is_connected = False
            client_socket = None
            update_gui_status(f"An unexpected error occurred: {e}", "red")
            show_message_box_thread_safe("Connection Error", f"An unexpected error occurred during connection: {e}", "error")
            root.after(0, lambda: set_button_states(tk.DISABLED)) # Ensure buttons are disabled

    connection_thread = threading.Thread(target=_connect)
    connection_thread.daemon = True # Allow the thread to close with the main application
    connection_thread.start()

def disconnect_tcp():
    """Disconnects from the TCP server."""
    global client_socket, is_connected
    if is_connected and client_socket:
        try:
            # It's better to try to send a disconnect message to the server if your protocol supports it
            # For now, just close the socket.
            client_socket.shutdown(socket.SHUT_RDWR) # Attempt to shut down both read/write
            client_socket.close()
        except OSError as e:
            print(f"Error closing socket (might already be closed): {e}") # Socket might already be closed by remote
        finally:
            is_connected = False
            client_socket = None
            toggle_connection_button.config(text="Connect")
            update_gui_status("Disconnected", "grey")
            show_message_box_thread_safe("Connection", "Disconnected from microcontroller.")
            set_button_states(tk.DISABLED) # Disable buttons after disconnection
    else:
        show_message_box_thread_safe("Connection Status", "Not connected.")

def toggle_tcp_connection():
    """Toggles the TCP connection status."""
    if is_connected:
        disconnect_tcp()
    else:
        connect_tcp()

# --- GUI Logic ---

def set_color_state(column_index, color_index):
    """
    Sets the state of relays for a specific column and color,
    ensuring only one color is active.
    """
    global column_active_colors

    # Check if currently connected before attempting to send commands
    if not is_connected:
        messagebox.showwarning("Connection Required", "Please connect to the microcontroller first.")
        return

    # Turn off all other relays in this column
    for i in range(3):
        if i != color_index:
            send_relay_command(column_index, i, 0) # 0 for OFF

    # Turn on the selected relay
    send_relay_command(column_index, color_index, 1) # 1 for ON

    # Update GUI indicator and internal state
    column_active_colors[column_index] = color_index
    update_color_indicators()

def update_color_indicators():
    """Updates the background color of the indicator labels."""
    for col_idx, active_color_idx in enumerate(column_active_colors):
        color_name = colors[active_color_idx]
        color_indicator_labels[col_idx].config(bg=color_name.lower()) # Tkinter uses lowercase color names

def next_color(column_index):
    """Cycles to the next color in the column."""
    if not is_connected:
        messagebox.showwarning("Connection Required", "Please connect to the microcontroller first.")
        return
    current_color_idx = column_active_colors[column_index]
    new_color_idx = (current_color_idx + 1) % 3
    set_color_state(column_index, new_color_idx)

def prev_color(column_index):
    """Cycles to the previous color in the column."""
    if not is_connected:
        messagebox.showwarning("Connection Required", "Please connect to the microcontroller first.")
        return
    current_color_idx = column_active_colors[column_index]
    new_color_idx = (current_color_idx - 1 + 3) % 3
    set_color_state(column_index, new_color_idx)

def save_subtitles():
    """Saves the current subtitle text to the config file."""
    config = {}
    for i, entry in enumerate(subtitle_entries):
        config[f"subtitle_{i}"] = entry.get()
    save_config(config)
    messagebox.showinfo("Configuration Saved", "Subtitles have been saved.")

def load_subtitles():
    """Loads subtitles from the config file on startup."""
    config = load_config()
    for i, entry in enumerate(subtitle_entries):
        # Use .get() with a default value to handle cases where a subtitle might not be in the config
        subtitle_text = config.get(f"subtitle_{i}", default_subtitles[i] if i < len(default_subtitles) else f"Column {i+1}")
        entry.delete(0, tk.END)
        entry.insert(0, subtitle_text)


# --- Main Application Window ---
root = tk.Tk()
root.title("Andon Control Panel")
root.geometry("1200x700") # Adjusted initial size to be wider and taller
root.resizable(True, True) # Made window resizable
root.configure(bg="#2E2E2E") # Dark background for the window

# --- Title Frame ---
title_frame = tk.Frame(root, bg="#3A3A3A", pady=10)
title_frame.pack(fill=tk.X, pady=(10, 0))

title_label = tk.Label(title_frame, text="Andon Control Panel", font=("Arial", 24, "bold"), fg="white", bg="#3A3A3A")
title_label.pack()

# --- Main Content Frame ---
content_frame = tk.Frame(root, bg="#2E2E2E", padx=20, pady=20)
content_frame.pack(expand=True, fill=tk.BOTH)

# --- Column Data ---
columns = ["VISUAL", "BASIS WEIGHT", "TENSILE", "AIR PERMIABILITY", "HYDRO PRESSURE", "SURFACTANT"]
colors = ["Red", "Yellow", "Green"]
default_subtitles = columns[:] # Initialize default subtitles

# Store references to widgets
subtitle_entries = []
color_indicator_labels = []
color_buttons = [[] for _ in range(len(columns))]
up_buttons = []
down_buttons = []

# Keep track of active color per column (0:Red, 1:Yellow, 2:Green)
column_active_colors = [0] * len(columns) # Initialize all to Red (index 0)

for col_idx, col_name in enumerate(columns):
    # Column Frame
    col_frame = tk.Frame(content_frame, bg="#4A4A4A", bd=2, relief="groove", padx=10, pady=10)
    # Using sticky="nsew" so the frame expands within its grid cell
    col_frame.grid(row=0, column=col_idx, padx=10, pady=10, sticky="nsew")

    # Subtitle Entry
    subtitle_entry = tk.Entry(col_frame, font=("Arial", 12), justify="center", bg="#5A5A5A", fg="white", insertbackground="white")
    # Make entry fill available horizontal space
    subtitle_entry.pack(fill=tk.X, pady=(0, 5))
    subtitle_entry.insert(0, col_name) # Default text
    subtitle_entries.append(subtitle_entry)

    # Color Indicator Label
    color_indicator = tk.Label(col_frame, text="", bg="grey", height=3, width=15, relief="sunken", bd=2)
    # Make indicator expand horizontally, maintaining aspect if possible (but width is fixed for now)
    color_indicator.pack(pady=5, fill=tk.X) # Fill X to help with resizing
    color_indicator_labels.append(color_indicator)

    # Color Buttons
    button_frame = tk.Frame(col_frame, bg="#4A4A4A")
    button_frame.pack(pady=5, fill=tk.X) # Make button frame fill X

    for color_idx, color_name in enumerate(colors):
        btn = tk.Button(button_frame, text=color_name, bg=color_name.lower(), fg="black",
                        font=("Arial", 10, "bold"), width=12, height=2,
                        command=lambda ci=col_idx, color_i=color_idx: set_color_state(ci, color_i),
                        state=tk.DISABLED) # Disable until connected
        btn.pack(pady=2, fill=tk.X, expand=True) # Make buttons fill X and expand
        color_buttons[col_idx].append(btn)

    # Up/Down Arrows
    arrow_frame = tk.Frame(col_frame, bg="#4A4A4A")
    arrow_frame.pack(pady=10, fill=tk.X) # Make arrow frame fill X

    up_arrow = tk.Button(arrow_frame, text="▲", font=("Arial", 16, "bold"), bg="#6A6A6A", fg="white",
                         command=lambda ci=col_idx: next_color(ci), width=3, height=1, state=tk.DISABLED) # Disable until connected
    up_arrow.pack(side=tk.LEFT, padx=5, fill=tk.X, expand=True) # Fill X and expand
    up_buttons.append(up_arrow)

    down_arrow = tk.Button(arrow_frame, text="▼", font=("Arial", 16, "bold"), bg="#6A6A6A", fg="white",
                           command=lambda ci=col_idx: prev_color(ci), width=3, height=1, state=tk.DISABLED) # Disable until connected
    down_arrow.pack(side=tk.RIGHT, padx=5, fill=tk.X, expand=True) # Fill X and expand
    down_buttons.append(down_arrow)

# Configure column weights for resizing
for i in range(len(columns)):
    content_frame.grid_columnconfigure(i, weight=1)

# Ensure rows in content_frame can expand vertically
content_frame.grid_rowconfigure(0, weight=1)


# --- Connection and Status Bar ---
connection_frame = tk.Frame(root, bg="#3A3A3A", pady=10)
connection_frame.pack(fill=tk.X, side=tk.BOTTOM, pady=(0, 10))

toggle_connection_button = tk.Button(connection_frame, text="Connect", font=("Arial", 12, "bold"),
                                     bg="#007BFF", fg="white", command=toggle_tcp_connection)
toggle_connection_button.pack(side=tk.LEFT, padx=20)

status_label = tk.Label(connection_frame, text="Disconnected", font=("Arial", 12), fg="grey", bg="#3A3A3A")
status_label.pack(side=tk.LEFT, padx=10, fill=tk.X, expand=True) # Make status label expand

save_config_button = tk.Button(connection_frame, text="Save Subtitles", font=("Arial", 12, "bold"),
                                bg="#28A745", fg="white", command=save_subtitles)
save_config_button.pack(side=tk.RIGHT, padx=20)

# --- Initial Setup ---
load_subtitles() # Load subtitles on startup
update_color_indicators() # Initialize indicator colors
set_button_states(tk.DISABLED) # Ensure buttons are disabled initially

# When the window is closed, ensure disconnection
def on_closing():
    if messagebox.askokcancel("Quit", "Do you want to quit and disconnect?"):
        disconnect_tcp()
        root.destroy()

root.protocol("WM_DELETE_WINDOW", on_closing)

root.mainloop()
