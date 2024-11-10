'''
This Script will:
    - send command to receive tcp/ip adc values directly from the Controller
    - run it through the AI model to determine if it's a distress call
    - send result back to MCU
'''
import socket
import wave

controller_IP = '192.168.1.8'
controller_PORT = 1234
SAMPLING_RATE = 8000
SAMPLE_DURATION = 6

# Create a TCP server
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.connect((controller_IP, controller_PORT))
print("Connection to Pico W Successful!")

# Collect data for the specified duration
# sample_count = sampling_rate * duration_seconds
try:
    while True:
        data = sock.recv(1024)  # Each ADC reading is 2 bytes
        print(data.decode())

    # if data:
    #     sample = struct.unpack('>H', data)[0]  # Convert bytes to integer
    #     samples.append(sample)
    # else:
    #     break
finally:
    sock.close()

