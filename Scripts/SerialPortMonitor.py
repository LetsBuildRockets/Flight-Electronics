from __future__ import print_function
import serial

ser = serial.Serial('COM5', 57600, timeout=1)
print(ser.name) 


TELEMETRY_MSG_TYPE=["MSG_ERROR","MSG_WARNING","MSG_INFO","MSG_GPS","MSG_IMU","MSG_POWER"]
while True:    
    command = ser.readline().split(b'\0',1)[0]
    length = len(str(command))
    if length > 1:
        print (TELEMETRY_MSG_TYPE[ord(command[0])],command[1:-1])
