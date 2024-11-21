
import tkinter as tk
from tkinter import ttk
from tkinter.messagebox import showerror
from relay_board_control import RelayBoard  # Assuming your RelayBoard class is in RelayBoard.py

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

class RelayBoardPage(tk.Toplevel):
    def __init__(self, master, board):
        super().__init__(master)
        self.board = board
        self.title(f"Relay Board {board.board_id}")
        self.geometry("500x400")

        ttk.Label(self, text=f"Relay Board {board.board_id}", font=("Arial", 16)).pack(pady=10)

        # Relay control switches
        self.relay_switches = []
        for relay_id in range(1, RelayBoard.MAX_RELAY_NUM + 1):
            frame = ttk.Frame(self)
            frame.pack(pady=5, fill=tk.X)

            ttk.Label(frame, text=f"Relay {relay_id}", width=10).pack(side=tk.LEFT, padx=5)

            var = tk.IntVar()
            var.set(int(self.board.value(relay_id)))  # Initialize with current relay state

            switch = ttk.Checkbutton(
                frame,
                variable=var,
                command=lambda r_id=relay_id, var=var: self.toggle_relay(r_id, var)
            )
            switch.pack(side=tk.LEFT)

            self.relay_switches.append(var)

    def toggle_relay(self, relay_id, var):
        """Toggle a relay and update its state."""
        try:
            new_state = var.get()
            self.board.value(relay_id, new_state)
        except Exception as e:
            showerror("Error", f"Failed to toggle relay {relay_id}: {e}")

app = RelayBoardGUI()
