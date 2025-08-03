# relays.py

import machine
from config import configurations

class RelayController:
    """
    Manages a collection of relays connected to the Raspberry Pi Pico's GPIO pins.
    """
    def __init__(self):
        """
        Initializes the controller for a specified number of relays.
        
        Args:
            num_relays (int): The number of relays to control, starting from GP0.
                              Defaults to 20 (GP0 to GP19).
        """
        if configurations.get("allowed_pins", None) != None:
            self.num_relays = len(configurations["allowed_pins"])
            # Create a list of Pin objects for each relay
            self.pins = [machine.Pin(i, machine.Pin.OUT) for i in configurations["allowed_pins"]]
            
            # Ensure all relays are off on startup
            self.all_off()
            print(f"RelayController initialized for {self.num_relays} relays.")

        elif configurations.get("num_relays", None) != None:
            self.num_relays = configurations.get("num_relays", None)

            if not (0 < self.num_relays <= 20):
                #TODO: implement some method to detect invalid pin ranges
                raise ValueError("Number of relays must be between 1 and 20.")
                
            # Create a list of Pin objects for each relay
            self.pins = [machine.Pin(i, machine.Pin.OUT) for i in range(self.num_relays)]
            
            # Ensure all relays are off on startup
            self.all_off()
            print(f"RelayController initialized for {self.num_relays} relays.")

        else:
            raise ValueError("must define allowed_pins list or num_relays in the configurations dict")

    def set_relay(self, index: int, state: int) -> bool:
        """
        Sets a specific relay to a given state (ON or OFF).

        Args:
            index (int): The index of the relay to control (0 to 19).
            state (int): The desired state (1 for ON, 0 for OFF).
        
        Returns:
            bool: True if the command was successful, False otherwise.
        """
        # Validate input
        if not (0 <= index < self.num_relays):
            print(f"Error: Relay index {index} is out of bounds.")
            return False
        
        if state not in [0, 1]:
            print(f"Error: Invalid state {state}. Must be 0 or 1.")
            return False
            
        # Set the pin value
        try:
            self.pins[index].value(state)
            print(f"Relay {index} set to {'ON' if state == 1 else 'OFF'}")
            return True
        except Exception as e:
            print(f"Failed to set relay {index}: {e}")
            return False

    def all_off(self):
        """Turns all connected relays off."""
        print("Turning all relays OFF.")
        for pin in self.pins:
            pin.value(0)
