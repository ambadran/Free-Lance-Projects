from station import Station
# from server import Server
import _thread

def controller_core():
    """
    Main Routine for second core 
    send and receive commands through nrf
    """
    station = Station()


def server_core():
    '''
    Main Routine for first core
    Host Web App
    '''
    server = Server()

s = Station()
