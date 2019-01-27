import serial
import atexit
import socket

HOST = ''
PORT = 50001

#s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
#s.bind((HOST, PORT))
#s.listen(10)
#print ('wating for IMU socket client to connect')
#conn, addr = s.accept()
#print('Connected by', addr)

ser = serial.Serial('COM5', 57600, timeout=1)
print(ser.name) 
    
TELEMETRY_MSG_TYPE=["MSG_IGNORE","MSG_ERROR","MSG_WARNING","MSG_INFO","MSG_GPS","MSG_IMU","MSG_POWER"]

def exit_handler():
    ser.flush()
    ser.close()
atexit.register(exit_handler)
while True:    
    command = (ser.readline().split(b'\0',1)[0]).rstrip()
    length = len(str(command))
    if length > 3:
        print (TELEMETRY_MSG_TYPE[command[0]],str(command[1:])[2:-1])
        #if command[0] == 5:
        #    conn.sendall(('Orientation: '+str(command[1:])[2:-1]+'\n').encode())
