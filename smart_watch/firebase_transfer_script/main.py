'''
Reads from upy_device
parses upy_device data
sends to firebase
'''
import serial
import time
import threading
import firebase_admin
from firebase_admin import credentials, firestore

############################## Setting up MicroPython Device ##############################
SERIAL_PORT = '/dev/ttyACM1'  # MUST FIGURE OUT
BAUD_RATE = 115200

data_dict_global = {
                "hr": 0,
                "spo2": 0,
                "x": 0,
                "y": 1,
                "z": 0
        }  # the Global dict that will be set by upy_device and sent to firebase

def read_from_device(ser):
    try:
        while True:
            if ser.is_open:
                if ser.in_waiting > 0:
                    data = ser.read(144).decode('utf-8')

                    if 'MPU' in data and 'GPS' in data:
                        # print(data)
                        # print('ksldjfk\n\n')
                        start_ind = data.find('MPU')
                        first_close_bracket = data.find(')')
                        second_close_bracket = data.find(')', first_close_bracket+1)
                        if second_close_bracket == -1:
                            return None
                        end_ind = second_close_bracket+1

                        string = data[start_ind:end_ind]
                        temp_dict = parse(string)
                        if temp_dict is not None:
                            print(f"Receved:\n{string}\nand Parsed:\n{temp_dict}\n\n")
                            global data_dict_global
                            data_dict_global.update(temp_dict)

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

def parse(string):
    '''
    parses the output of the micropython device 
    '''
    try:
        data = string.split('\n')

        data_splitted = []
        for datum in data:
            data_splitted.append(datum.split(':'))

        mpu_values = eval(data_splitted[0][1])
        heart_value = data_splitted[1][1]
        heart_value = eval(heart_value[:heart_value.find('BPM')])

        gps_values = eval(data_splitted[2][1])

        data_dict = {
                "hr": heart_value,
                "spo2": 0,
                "x": mpu_values[0],
                "y": mpu_values[1],
                "z": 0
                }

        return data_dict

    except Exception:
        # print(f"couldn't parse string: {string}")
        return None


#########################################################################################


############################## Settting up Fire base ##############################
cred = credentials.Certificate(r"graduation-f2c3d-firebase-adminsdk-ov1ww-0967297188.json")
firebase_admin.initialize_app(cred)
db=firestore.client()
DeviceID="abc123"

#real time metrics collection
realtimemetrics=db.collection('realTimeMetrics').where('deviceId','==',DeviceID).get()
for metric in realtimemetrics:
    id=metric.id
realtimemetrics=db.collection('realTimeMetrics').document(id)

dailyhealth=db.collection('dailyHealth').where('deviceID','==',DeviceID).get()
for health in dailyhealth:
    id=health.id
    dailyhealth_data=health.to_dict()
dailyhealth=db.collection('dailyHealth').document(id)

def realtimemetricsUpdate(data):
    # data={
    #     'hr':hr,
    #     'spo2':spo2,
    #     'x':x,
    #     'y':y,
    #     'z':z
    # }
    realtimemetrics.update(data)

def dailyHealthUpdate(hr, spo2):
    HR=dailyhealth_data.get('HR')
    HR.append(hr)
    SPO2=dailyhealth_data.get('SPO2')
    SPO2.append(spo2)
    data={
        'HR':HR,
        'SPO2':SPO2
    }
    dailyhealth.update(data)
##########################################################################################


############################## Main Routine ##############################
def main():
    ser = None
    try:
        # Open serial connection
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        time.sleep(2)  # Give some time for the device to reset

        try:
            global data_dict_global
            # Reading from the device in a separate thread
            read_thread = threading.Thread(target=read_from_device, args=(ser,))
            read_thread.daemon = True
            read_thread.start()

            # Sending initial commands to the device
            write_to_device(ser, 'from main import main')
            write_to_device(ser, 'main()')

            # Keep the main thread alive to maintain the program running
            while True:
                time.sleep(10)

                print(f"Sending {data_dict_global} to firebase!")
                realtimemetricsUpdate(data_dict_global)
                dailyHealthUpdate(data_dict_global['hr'], data_dict_global['spo2'])

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

##########################################################################################

if __name__ == '__main__':
    main()

