
string = """MPU: (-173,99)
Heart: 0.0BPM
GPS: (24.9807, 35.606)"""


def parse(string):
    '''
    parses the output of the micropython device 
    '''
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
            "x": gps_values[0]
            "y": gps_values[1]
            "z": 0
            }

    return data_dict

