'''
Terminal Abstraction to process commands coming through tcp connection
'''
import re

class Terminal:
    '''
    Terminal command executing abstraction
    '''
    def __init__(self, server, relays):
        self.server = server
        self.relays = relays

    def print(self, string):
        '''
        prints text both in micropython terminal and in tcp connection
        '''
        print(string)
        self.server.client.sendall(f"{string}\n\n")

    def __call__(self, cmd):
        '''
        Process Commands
        '''
        if cmd.isspace():
            print('ok..')  # testing connection
            return None

        ### Step 1:  identifying command
        cmd = cmd.strip()
        command_patterns = re.compile(r'R(-?\d+)V?(\d?)')
        matched = command_patterns.match(cmd)
        if matched:

            ### Step 2: check for logical erros
            # check if relay id is out of range
            pin_value = int(matched.group(1))
            if pin_value > len(self.relays) or pin_value < -1:
                self.print(f"Error: Relay id range: 0-{len(self.relays)}")
                return None

            # relay value is either 0 or 1
            if matched.group(2) == '':
                value = None
            else:
                value = int(matched.group(2))
                if value != 1 and value != 0:
                    self.print("Error: relay value is either 0 or 1")
                    return None

            ### Step 3: Execution
            if pin_value != -1:
                relay_value = self.relays.value(pin_value, value)
                self.print(f"R{pin_value}V{relay_value}")

            elif value is None:
                self.print(str(self.relays))

            elif value == 0:
                self.relays.off()
                self.print(str(self.relays))

            elif value == 1:
                #TODO: send warning
                self.print("Turning ALL relays ON is unsafe!!!")


        else:
            print(f"Unknown command received: {cmd}")

 
