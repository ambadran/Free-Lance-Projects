'''

'''
import tkinter as tk
from tkinter import ttk, simpledialog
from tkinter.messagebox import showerror
from relay_board_control import RelayBoard  # Assuming your RelayBoard class is in RelayBoard.py
import json
import re

class RelayBoardGUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title("ESP32 Relay Control")
        self.geometry("600x400")
        
        self.boards = {}  # Dictionary to store connected RelayBoard objects
        self.initialize_home_page()

    def initialize_home_page(self):
        """Initialize the home page for discovering and connecting to relay boards."""
        # Clear existing widgets
        for widget in self.winfo_children():
            widget.destroy()

        # Home page widgets
        ttk.Label(self, text="ESP32 Relay Control Home", font=("Arial", 16)).pack(pady=10)
        ttk.Label(self, text="Discover and connect to ESP32 Relay Boards.").pack(pady=5)

        # Board ID input
        ttk.Label(self, text="Board ID:").pack(pady=5)
        self.board_id_var = tk.StringVar()
        ttk.Entry(self, textvariable=self.board_id_var).pack(pady=5)

        # Connect button
        ttk.Button(self, text="Connect", command=self.connect_to_board).pack(pady=5)

        # IP Configuration Dialog
        ttk.Button(self, text="Configure IP", command=self.open_ip_config).pack(pady=10)

        # Connected boards list
        self.connected_boards_frame = ttk.Frame(self)
        self.connected_boards_frame.pack(pady=20, fill=tk.BOTH, expand=True)
        self.refresh_connected_boards()

    def connect_to_board(self):
        """Attempt to connect to a relay board."""
        try:
            board_id = int(self.board_id_var.get())
            if board_id in self.boards:
                showerror("Error", f"Board {board_id} is already connected.")
                return

            # Initialize RelayBoard and test connection
            board = RelayBoard(board_id)
            self.boards[board_id] = board

            # Success
            self.refresh_connected_boards()
            showerror("Success", f"Connected to board {board_id}!")

        except Exception as e:
            showerror("Connection Error", f"Failed to connect to board: {e}")

    def open_ip_config(self):
        """Open the IP Configuration Dialog."""
        board_id = simpledialog.askinteger("Board ID", "Enter Board ID:")
        if board_id is not None:
            IPConfigDialog(self, board_id)

    def refresh_connected_boards(self):
        """Update the list of connected boards."""
        for widget in self.connected_boards_frame.winfo_children():
            widget.destroy()

        ttk.Label(self.connected_boards_frame, text="Connected Boards:", font=("Arial", 12)).pack(anchor="w")
        for board_id in self.boards:
            ttk.Button(
                self.connected_boards_frame,
                text=f"Board {board_id}",
                command=lambda b_id=board_id: self.open_board_page(b_id)
            ).pack(pady=5, anchor="w")

    def open_board_page(self, board_id):
        """Open the page for a specific relay board."""
        board = self.boards[board_id]
        RelayBoardPage(self, board)

class IPConfigDialog(tk.Toplevel):
    def __init__(self, master, board_id):
        super().__init__(master)
        self.board_id = board_id
        self.title(f"Configure IP for Board {board_id}")
        self.geometry("300x150")

        ttk.Label(self, text=f"Enter IP for Board {board_id}:").pack(pady=10)

        self.ip_var = tk.StringVar()
        ttk.Entry(self, textvariable=self.ip_var, width=25).pack(pady=5)

        ttk.Button(self, text="Save", command=self.save_ip).pack(pady=10)

    def save_ip(self):
        """
        Save the manually entered IP.
        """
        ip_address = self.ip_var.get()
        if self.validate_ip(ip_address):
            RelayBoard.save_ip_address(self.board_id, ip_address)
            tk.messagebox.showinfo("Success", f"IP address for board {self.board_id} saved!")
            self.destroy()
        else:
            tk.messagebox.showerror("Error", "Invalid IP address. Please try again.")

    @staticmethod
    def validate_ip(ip):
        """
        Validate the entered IP address format 
        """
        pattern = re.compile(
            r"^(?:[0-9]{1,3}\.){3}[0-9]{1,3}$"
        )  # Basic IPv4 validation
        return pattern.match(ip) is not None

class RelayBoardPage(tk.Toplevel):
    DESCRIPTION_FILE_TEMPLATE = "relayboard_{board_id}.json"

    def __init__(self, master, board):
        super().__init__(master)
        self.board = board
        self.title(f"Relay Board {board.board_id}")
        self.geometry("600x700")

        # Load existing descriptions
        self.description_file = self.DESCRIPTION_FILE_TEMPLATE.format(board_id=board.board_id)
        self.relay_descriptions = self.load_descriptions()

        ttk.Label(self, text=f"Relay Board {board.board_id}", font=("Arial", 16)).pack(pady=10)

        # Relay switches and entries
        self.switch_vars = {}
        self.switch_frames = ttk.Frame(self)
        self.switch_frames.pack(fill=tk.BOTH, expand=True)

        for relay_id in range(RelayBoard.MAX_RELAY_NUM):  # Relay IDs start from 0
            self.create_relay_control(relay_id)

    def create_relay_control(self, relay_id):
        """Create a control row with label, entry, and toggle switch."""
        frame = ttk.Frame(self.switch_frames)
        frame.pack(pady=5, fill=tk.X)

        # Label for the relay ID
        ttk.Label(frame, text=f"Relay {relay_id}:", width=15).pack(side=tk.LEFT, padx=5)

        # Entry for relay description
        description_var = tk.StringVar(value=self.relay_descriptions.get(str(relay_id), ""))
        entry = ttk.Entry(frame, textvariable=description_var, width=30)
        entry.pack(side=tk.LEFT, padx=10)

        # Save description whenever it changes
        description_var.trace_add("write", lambda *_: self.save_description(relay_id, description_var.get()))

        # Get the current relay state
        try:
            current_state = int(self.board.value(relay_id))  # Assuming 0 for OFF and 1 for ON
        except Exception:
            current_state = 0  # Default to OFF if there's an error

        # Create the toggle switch
        var = tk.BooleanVar(value=bool(current_state))
        self.switch_vars[relay_id] = var

        switch = tk.Canvas(frame, width=60, height=30, bg="lightgray", highlightthickness=0)
        switch.pack(side=tk.LEFT, padx=10)

        # Draw the toggle
        self.draw_toggle(switch, var)

        # Bind click event
        switch.bind("<Button-1>", lambda event, r_id=relay_id, var=var: self.toggle_relay(r_id, var, switch))

    def draw_toggle(self, canvas, var):
        """Draw the switch on the canvas."""
        canvas.delete("all")
        state = var.get()

        # Draw background (green for ON, red for OFF)
        canvas.create_rectangle(0, 0, 60, 30, fill="green" if state else "red", outline="")

        # Draw the toggle circle
        canvas.create_oval(
            5 if state else 35, 5, 25 if state else 55, 25,
            fill="white",
            outline="",
        )

    def toggle_relay(self, relay_id, var, canvas):
        """Toggle relay state and update its UI."""
        try:
            new_state = not var.get()
            var.set(new_state)

            # Update the switch UI
            self.draw_toggle(canvas, var)

            # Send the new state to the relay board
            self.board.value(relay_id, int(new_state))
        except Exception as e:
            tk.messagebox.showerror("Error", f"Failed to toggle relay {relay_id}: {e}")

    def load_descriptions(self):
        """Load relay descriptions from a JSON file."""
        try:
            with open(self.description_file, "r") as file:
                return json.load(file)
        except FileNotFoundError:
            return {}  # No descriptions saved yet
        except json.JSONDecodeError:
            tk.messagebox.showerror("Error", "Failed to load descriptions. File may be corrupted.")
            return {}

    def save_description(self, relay_id, description):
        """Save the description of a relay to the JSON file."""
        self.relay_descriptions[str(relay_id)] = description
        try:
            with open(self.description_file, "w") as file:
                json.dump(self.relay_descriptions, file, indent=4)
        except Exception as e:
            tk.messagebox.showerror("Error", f"Failed to save description for relay {relay_id}: {e}")

app = RelayBoardGUI()
app.mainloop()
