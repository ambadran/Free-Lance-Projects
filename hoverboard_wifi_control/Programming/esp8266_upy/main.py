'''
HoverBoard Controlling using signal from WiFi
'''
from server import Server

# Main Routine
sleep(3)
server = Server()

while True:

    ### Host Server ###
    server.wait_for_client()
    server.handle_html_request(server.identify_html_request())

    ### Show in UART ###
    # print(server.actuators_dict)
    # print(server.sensors_dict)

    ### Toggle LED to show System is Operational ###
    server.led.value(not server.led.value())

