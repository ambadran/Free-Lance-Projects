from machine import Pin, I2C, Timer 
import time
import struct
from math import atan, degrees, atan2, asin, acos, atan, pi, sqrt, sin , cos, tan
import sys


# Register Definitions
XG_OFFS_TC       =  const(0x00)
YG_OFFS_TC       =  const(0x01)
ZG_OFFS_TC       =  const(0x02)
X_FINE_GAIN      =  const(0x03)
Y_FINE_GAIN      =  const(0x04)
Z_FINE_GAIN      =  const(0x05)
XA_OFFS_H        =  const(0x06)
XA_OFFS_L_TC     =  const(0x07)
YA_OFFS_H        =  const(0x08)
YA_OFFS_L_TC     =  const(0x09)
ZA_OFFS_H        =  const(0x0A)
ZA_OFFS_L_TC     =  const(0x0B)
XG_OFFS_USRH     =  const(0x13)
XG_OFFS_USRL     =  const(0x14)
YG_OFFS_USRH     =  const(0x15)
YG_OFFS_USRL     =  const(0x16)
ZG_OFFS_USRH     =  const(0x17)
ZG_OFFS_USRL     =  const(0x18)
SMPLRT_DIV       =  const(0x19)
CONFIG           =  const(0x1A)
GYRO_CONFIG      =  const(0x1B)
ACCEL_CONFIG     =  const(0x1C)
FF_THR           =  const(0x1D)
FF_DUR           =  const(0x1E)
MOT_THR          =  const(0x1F)
MOT_DUR          =  const(0x20)
ZRMOT_THR        =  const(0x21)
ZRMOT_DUR        =  const(0x22)
FIFO_EN          =  const(0x23)
I2C_MST_CTRL     =  const(0x24)
I2C_SLV0_ADDR    =  const(0x25)
I2C_SLV0_REG     =  const(0x26)
I2C_SLV0_CTRL    =  const(0x27)
I2C_SLV1_ADDR    =  const(0x28)
I2C_SLV1_REG     =  const(0x29)
I2C_SLV1_CTRL    =  const(0x2A)
I2C_SLV2_ADDR    =  const(0x2B)
I2C_SLV2_REG     =  const(0x2C)
I2C_SLV2_CTRL    =  const(0x2D)
I2C_SLV3_ADDR    =  const(0x2E)
I2C_SLV3_REG     =  const(0x2F)
I2C_SLV3_CTRL    =  const(0x30)
I2C_SLV4_ADDR    =  const(0x31)
I2C_SLV4_REG     =  const(0x32)
I2C_SLV4_DO      =  const(0x33)
I2C_SLV4_CTRL    =  const(0x34)
I2C_SLV4_DI      =  const(0x35)
I2C_MST_STATUS   =  const(0x36)
INT_PIN_CFG      =  const(0x37)
INT_ENABLE       =  const(0x38)
DMP_INT_STATUS   =  const(0x39)
INT_STATUS       =  const(0x3A)
ACCEL_XOUT_H     =  const(0x3B)
ACCEL_XOUT_L     =  const(0x3C)
ACCEL_YOUT_H     =  const(0x3D)
ACCEL_YOUT_L     =  const(0x3E)
ACCEL_ZOUT_H     =  const(0x3F)
ACCEL_ZOUT_L     =  const(0x40)
TEMP_OUT_H       =  const(0x41)
TEMP_OUT_L       =  const(0x42)
GYRO_XOUT_H      =  const(0x43)
GYRO_XOUT_L      =  const(0x44)
GYRO_YOUT_H      =  const(0x45)
GYRO_YOUT_L      =  const(0x46)
GYRO_ZOUT_H      =  const(0x47)
GYRO_ZOUT_L      =  const(0x48)
EXT_SENS_DATA_00 =  const(0x49)
EXT_SENS_DATA_01 =  const(0x4A)
EXT_SENS_DATA_02 =  const(0x4B)
EXT_SENS_DATA_03 =  const(0x4C)
EXT_SENS_DATA_04 =  const(0x4D)
EXT_SENS_DATA_05 =  const(0x4E)
EXT_SENS_DATA_06 =  const(0x4F)
EXT_SENS_DATA_07 =  const(0x50)
EXT_SENS_DATA_08 =  const(0x51)
EXT_SENS_DATA_09 =  const(0x52)
EXT_SENS_DATA_10 =  const(0x53)
EXT_SENS_DATA_11 =  const(0x54)
EXT_SENS_DATA_12 =  const(0x55)
EXT_SENS_DATA_13 =  const(0x56)
EXT_SENS_DATA_14 =  const(0x57)
EXT_SENS_DATA_15 =  const(0x58)
EXT_SENS_DATA_16 =  const(0x59)
EXT_SENS_DATA_17 =  const(0x5A)
EXT_SENS_DATA_18 =  const(0x5B)
EXT_SENS_DATA_19 =  const(0x5C)
EXT_SENS_DATA_20 =  const(0x5D)
EXT_SENS_DATA_21 =  const(0x5E)
EXT_SENS_DATA_22 =  const(0x5F)
EXT_SENS_DATA_23 =  const(0x60)
MOT_DETECT_STATUS=  const(0x61)
I2C_SLV0_DO      =  const(0x63)
I2C_SLV1_DO      =  const(0x64)
I2C_SLV2_DO      =  const(0x65)
I2C_SLV3_DO      =  const(0x66)
I2C_MST_DELAY_CTRL= const(0x67)
SIGNAL_PATH_RESET=  const(0x68)
MOT_DETECT_CTRL  =  const(0x69)
USER_CTRL        =  const(0x6A)
PWR_MGMT_1       =  const(0x6B)
PWR_MGMT_2       =  const(0x6C)
BANK_SEL         =  const(0x6D)
MEM_START_ADDR   =  const(0x6E)
MEM_R_W          =  const(0x6F)
DMP_CFG_1        =  const(0x70)
DMP_CFG_2        =  const(0x71)
FIFO_COUNTH      =  const(0x72)
FIFO_COUNTL      =  const(0x73)
FIFO_R_W         =  const(0x74)
WHO_AM_I         =  const(0x75)


class MPU6050:
    
    accel_gyro_address = 104
    
    accel_sensitivity_values = [16384, 8192, 4096, 2048]
    gyro_sensitivity_values = [131, 65.5, 32.8, 16.4]
    
    def __init__(self, accel_sensitivity_ind, gyro_range_ind, i2c):
        """
        :param accel_range: int from 0-3, indicating accel range from datasheet
        :param gyro_range: int from 0-3, indicating gyro range from datasheet
        """
        self.i2c = i2c

        #################### setting the attributes ####################
        #raws
        self.raw_accel: list
        self.raw_gyro: list
        self.raw_temp: float
        
        # sensitivity and range
        self.accel_sensitivity_ind = accel_sensitivity_ind
        self.accel_sensitivity = MPU6050.accel_sensitivity_values[accel_sensitivity_ind]
        
        self.gyro_range_ind = gyro_range_ind
        self.gyro_range = MPU6050.gyro_sensitivity_values[gyro_range_ind]
        
        # offsets
        self.gyro_offset = [0, 0, 0]
        self.accel_offset = [-940, -200, 800]
        
        # No filter reading
        # angles_relative is the angle read by integrating the gyro only, nothing else
        self.angles_relative = [0, 0, 0]  # initializing as 0 values
        self.gyro_alone_period = 10  # in ms
        
        # Complementary Filter attributes
        self.complK = 0.2  # filter constant
        self.complK_I = 1 - self.complK  # the complementary of the constant
        self.compl_period = 10  # in ms
        self.angles_compl = self.gs_to_angles()  # initiate the angles by reading from the accelerometer only
        self.compl_gyro_reset_period = 5000  # 5seconds to reset gyro amount  #TODO: remove if not a good idea
        
        # Kalman Filter attributes
        #TODO:
        
        
        ##################### Initializing the Module ###########################
        # ACCELEROMETER AND GYROSCOPE SETTINGS
        if int(self.read(WHO_AM_I)) != 104:
            raise ValueError("accelerometer/gyroscope who_am_i returned wrong DeviceID")
        
        self.write(PWR_MGMT_1, 0b00000000)
        time.sleep(0.2)
        self.write(CONFIG, 0b00000011)
        self.write(SMPLRT_DIV, 0x04)
        self.write(GYRO_CONFIG, gyro_range_ind << 3)
        self.write(ACCEL_CONFIG, accel_sensitivity_ind << 3)
        self.write(INT_PIN_CFG, 0x02)  # i don't know if this will make problems
        self.write(FIFO_EN, 0b00000000)
        self.write(I2C_MST_CTRL, 0b00000000)
        self.write(INT_ENABLE, 0b00000000)
        self.write(I2C_MST_DELAY_CTRL, 0b00000000)
        self.write(PWR_MGMT_2, 0b00000000)
        
        
       
        time.sleep(0.2)
        
        
        # Calibration
        self.calibrate_gyro()
    
    
    ###########################################################################################################
    ################################### I2C and Specific Register Functions ###################################
    ###########################################################################################################
    
    ################################## ACCELOROMETER AND GYROSCOPE FUNCTIONS ##################################
    def read(self, register):  # meant for direct user interactions only
        return bin(ord(self.i2c.readfrom_mem(MPU6050.accel_gyro_address, register, 1)))

    def write(self, register, value):
        self.i2c.writeto_mem(MPU6050.accel_gyro_address, register, chr(value))
    
    def reset_all_registers(self):
        self.write(PWR_MGMT_1, 0b10000000)
        
    
    ###########################################################################################################
    ###################################### Read and Converting Raw Data #######################################
    ###########################################################################################################
        
    ############################################### GYROSCOPE #################################################
    @property
    def raw_gyro(self):
        """
        returns raw gyro numbers as read from device
        """
        byte_string = self.i2c.readfrom_mem(MPU6050.accel_gyro_address, GYRO_XOUT_H, 6)
        results = [byte_string[0]<<8 | byte_string[1], byte_string[2]<<8 | byte_string[3], byte_string[4]<<8 | byte_string[5]]
        for ind in range(3):
            if results[ind] > 32768:  # 10000000 00000000, aka a negative number
                results[ind] -= 65535  # make it a negative number
        return results
    
    @property
    def gyro(self):
        """
        returns gyro numbers in deg/s
        """
        byte_string = self.i2c.readfrom_mem(MPU6050.accel_gyro_address, GYRO_XOUT_H, 6)
        results = [byte_string[0]<<8 | byte_string[1], byte_string[2]<<8 | byte_string[3], byte_string[4]<<8 | byte_string[5]]
        for ind in range(3):
            if results[ind] > 32768:  # 10000000 00000000, aka a negative number
                results[ind] -= 65535  # make it a negative number
        return [round( (results[i] - self.gyro_offset[i])/self.gyro_range, 3) for i in range(3)]  #TODO: fix it
    
    
    ############################################### ACCELEROMTER #################################################
    @property
    def raw_accel(self):
        """
        returns raw accelerometer numbers as read from device
        """
        byte_string = self.i2c.readfrom_mem(MPU6050.accel_gyro_address, ACCEL_XOUT_H, 6)
        results = [byte_string[0]<<8 | byte_string[1], byte_string[2]<<8 | byte_string[3], byte_string[4]<<8 | byte_string[5]]
        for ind in range(3):
            if results[ind] > 32768:  # 10000000 00000000, aka a negative number
                results[ind] -= 65535  # make it a negative number
        return results
    
    @property
    def accel(self):
        """
        returns accelerometer numbers in g
        """
        byte_string = self.i2c.readfrom_mem(MPU6050.accel_gyro_address, ACCEL_XOUT_H, 6)
        results = [byte_string[0]<<8 | byte_string[1], byte_string[2]<<8 | byte_string[3], byte_string[4]<<8 | byte_string[5]]
        for ind in range(3):
            if results[ind] > 32768:  # 10000000 00000000, aka a negative number
                results[ind] -= 65535  # make it a negative number
                
        return [round( (results[i] + self.accel_offset[i]) / self.accel_sensitivity, 2) for i in range(3)]
    
        ######VERY IMPORTANT: I noticed that in the Ax there is an offset that push it towards the negative side #####################
    
    
    def gs_to_angles(self):
        """
        return roll and pitch data
        using data from accelerometer only
        NO yaw data since accel can't measure yaw data
        """
        gravity = self.accel
        
        # only use if you'll use asin()  ########### IMP ############
        # clamping the input gs
#         gravity[0] /= 0.981  
#         gravity[1] /= 0.981

        # clamping to values of 1 as we don't want any values other's than gravity's which will never exceed one g
        if gravity[0] > 1:  # asin domain is (-1 < x < 1)
            gravity[0] = 1
        elif gravity[0] < -1:
            gravity[0] = -1
        if gravity[1] > 1:  # asin domain is (-1 < x < 1)
            gravity[1] = 1
        elif gravity[1] < -1:
            gravity[1] = -1
            
#         angles = [asin(gravity[0]), asin(gravity[1])]  # works just fine BUT IS LIMITED TO -90 < THETA < 90, if it goes over 90 degrees roll or pitch, it will start decreasing in numbers
        
        angles = [atan2(gravity[1], gravity[2]), atan2(gravity[0], gravity[2])]  # compute roll and pitch relative to earth's gravity -180 <= roll/pitch <= 180
        return [int(degrees(angle)) for angle in angles]
    
    
    @property
    def raw_temp(self):
        out_h = self.read(TEMP_OUT_H)
        out_l = self.read(TEMP_OUT_L)
        #TODO: continue this
        
    @property
    def temp(self):
        #TODO
        pass
    ###########################################################################################################
    ###########################################################################################################
    ###########################################################################################################  
    
    
    ###########################################################################################################
    ########################################## No Filter Values ###############################################
    ###########################################################################################################
    @property
    def angles_absolute(self):
        """
        return absolute roll and pitch angle from accelerometer reading
        """
        result = self.gs_to_angles()
        return result
    
    def calc_gyro_alone(self, t):  # for timer to work there must be a dummy argument
        """
        calculates angle by integrating the gyro readings
        """
        
        gyro = self.gyro  # values as degree/sec
        for ind in range(len(self.angles_relative)):
            self.angles_relative[ind] += gyro[ind] * self.gyro_alone_period/1000
    
    def activate_gyro_reading(self):
        self.gyro_alone_timer = Timer(period = self.gyro_alone_period, mode = Timer.PERIODIC, callback = self.calc_gyro_alone)
    
    def deactivate_gyro_reading(self):
        self.gyro_alone_timer.deinit()
    
    ###########################################################################################################
    ###########################################################################################################
    ########################################################################################################### 
    
    
    
    ###########################################################################################################
    ####################################### Complementary Filter ##############################################
    ###########################################################################################################
    def calc_angle_compl(self, timer_arg):
        """
        returns angles of roll and pitch only by fusing data from gyro and accel using a complementary filter
        This function should be called every (self.comp_period) time period
        """
        # reading the mpu values as gs' and deg/sec
        accel_angles = self.gs_to_angles()  # Absolute Roll and Pitch
        gyro = self.gyro  # Roll, Pitch and Yaw derivatives
        
        # Roll and Pitch: absolute from acceleromter and relative from gyroscope
        for ind in range(2):  # roll and pitch only
            accel_side = self.complK * accel_angles[ind]
            gyro_side = self.complK_I * ( self.angles_compl[ind] + gyro[ind] * self.compl_period/1000 )
            self.angles_compl[ind] = accel_side + gyro_side
            
    def reset_gyro_compl(self):  #TODO: remove if it's a bad idea
        """
        reset if the gyro drifting value is too big
        """
        self.angles_compl = gs_to_angles(self.accel) 
        
        
    def activate_compl_filter(self):
        """
        starts a timer that executes complementary filter calculations every self.comp_period time period
        and a timer to reset gyro value every (self.compl_gyro_reset_period) time period
        """
        
        #  activating the timer that will execute the filter every compl_period time period
        if sys.platform == 'esp32':
            self.compl_timer = Timer(0)
            self.compl_timer.init(period = self.compl_period, mode = Timer.PERIODIC, callback = self.calc_angle_compl)

        else:
            self.compl_timer = Timer(period = self.compl_period, mode = Timer.PERIODIC, callback = self.calc_angle_compl)
        
        # activating a timer that will reset the gyro every (self.compl_gyro_reset_period) time period
#         self.gyro_reset_timer = Timer(period = self.compl_gyro_reset_period, mode = Timer.PERIODIC, callback = self.reset_gyro_compl)  #TODO: test this


    def deactivate_compl_filter(self):
        """
        stops the constant complementary filter reading and calculations
        """
        self.compl_timer.deinit()
    ###########################################################################################################
    ###########################################################################################################
    ###########################################################################################################  




    ###########################################################################################################
    ########################################## Kalman Filter ##################################################
    ###########################################################################################################

    def activate_kalman_filter(self):
        """
        starts a timer that executes kalman filter calculations every self.kalman_period time period
        """
        
        #  activating the timer that will execute the filter every self.kalman_period time period
        self.compl_timer = Timer(period = self.kalman_period, mode = Timer.PERIODIC, callback = self.calc_angle_kalman)
    
    def calc_angle_kalman(self, timer_argument):
        print("kalman filter")
        
    ###########################################################################################################
    ###########################################################################################################
    ###########################################################################################################
        
            
        
    
    ###########################################################################################################
    ####################################### Miscilaneous Functions ############################################
    ###########################################################################################################
    def calibrate_gyro(self):
        for _ in range(1024):
            raws = self.raw_gyro
            offset_added = [raws[i] - self.gyro_offset[i] for i in range(3)]
            offset_sum = [0, 0, 0]
            offset_sum = [offset_sum[i] + offset_added[i] for i in range(3)]
        
        self.gyro_offset = [offset_sum[i] for i in range(3)]
    
    def zero_accel(self):
        """
        #TODO: changes accel_offset to make it zero in current position
        """
        pass
    
       
    ###########################################################################################################
    ###########################################################################################################
    ###########################################################################################################
    
    
    ###########################################################################################################
    ############################################# EXPORTING DATA ##############################################
    ###########################################################################################################
    def export(self, angles):
        with open('log.txt', 'w') as f:
            f.write("{},{},{}".format(angles[0], angles[1], angles[2]))
    ###########################################################################################################
    ###########################################################################################################
    ###########################################################################################################

# mpu = MPU6050(0, 1)
# mpu.activate_compl_filter()

# while True:
#     print(mpu.angles_absolute)



