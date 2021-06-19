import serial
import time

serdev = '/dev/ttyUSB0'
s = serial.Serial(serdev, 9600, timeout = 3)

while True:                                 # 無窮迴圈
    char = s.read(1)                        # 讀板子回覆
    print(char.decode(), end = '')          # 顯示回覆

s.close()                                   # 結束