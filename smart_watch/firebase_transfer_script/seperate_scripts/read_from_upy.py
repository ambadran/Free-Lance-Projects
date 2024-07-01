
import serial
import time
import threading

# Replace with your actual serial port and baud rate
SERIAL_PORT = '/dev/ttyACM1'  # or '/dev/ttyUSB0' for Linux
BAUD_RATE = 115200

def read_from_device(ser):
    try:
        while True:
            if ser.is_open:
                if ser.in_waiting > 0:
                    data = ser.read(ser.in_waiting)
                    print(data.decode('utf-8'), end='')
            else:
                print("Serial port closed unexpectedly.")
                break
    except Exception as e:
        print(f"Error in read_from_device: {e}")

def write_to_device(ser, command):
    try:
        if ser.is_open:
            command = command + '\r\n'
            ser.write(command.encode('utf-8'))
        else:
            print("Serial port is not open.")
    except Exception as e:
        print(f"Error in write_to_device: {e}")

def main():
    ser = None
    try:
        # Open serial connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Give some time for the device to reset

        try:
            # Reading from the device in a separate thread
            read_thread = threading.Thread(target=read_from_device, args=(ser,))
            read_thread.daemon = True
            read_thread.start()

            # Sending initial commands to the device
            write_to_device(ser, 'from main import main')
            write_to_device(ser, 'main()')

            # Keep the main thread alive to maintain the program running
            while True:
                time.sleep(1)
                if not ser.is_open:
                    print("Serial port closed. Exiting.")
                    break

        finally:
            if ser and ser.is_open:
                ser.close()
    except Exception as e:
        print(f"Error in main: {e}")
    finally:
        if ser and ser.is_open:
            ser.close()

if __name__ == '__main__':
    main()
