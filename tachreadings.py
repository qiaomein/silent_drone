
#%%
import serial
import time
from time import perf_counter as tcounter
import numpy as np
from matplotlib import pyplot as plt
from scipy import fft

#### SETTINGS #########
timeframe = 90 # seconds to record
port_name = "COM7"
baudrate = 115200
interval = 5

##########################

print("Starting up...")
arduino = serial.Serial(port_name, baudrate)
arduino.close() 
arduino.open()

t0 = tcounter()


plt.close('all')
plt.figure()


# cmd: type in reference angular velocity in rad/s
w_ref = 3
triggered = False

def trigger(t_micro):
    global triggered
    global w_ref

    ts = t_micro/(1e6)
    if int(ts) % interval == 0 and not triggered:
        triggered = True
        w_ref += 2
        return True
    if int(ts) % interval != 0:
        triggered = False
    
    return False



print(f"Reading until t={timeframe}s")
t = 0
rawdata = []

while t < timeframe*1e6: # in microseconds
    
    data = arduino.readline()
    Sdata = data.decode().strip().split(",")
    #t, tach2, tach4, pwm, error, cumerror, rpm2, rpm4 = list(map(float,Sdata))
    #print(Sdata)
    try:
        t, tach2, tach4, pwm, error, rpm2, rpm4 = list(map(float,Sdata))
    except:
        t, tach2, tach4, pwm, error, rpm2, rpm4 = [0] * 7
        print("except!")
    cmd = "testing"
    if trigger(t):
        print("w_ref: ", w_ref)
        #arduino.write(cmd.encode())
    rawdata.append([t,tach2,tach4]) #time, pwm, error, cumerror, rpm2, rpm4
    
    #print(t)

#arduino.write("0\r".encode())
rawdata = np.array(rawdata)
#%%

plt.figure()
da = 3500
a,b = [da, da + 500]
tPlot = rawdata[a:b,0] * 1e-6
tach2 = rawdata[a:b,1]
tach4 = rawdata[a:b,2]
plt.plot(tPlot,tach2,tPlot,tach4)
plt.xlabel("time [s]"); plt.ylabel("tach readings")
plt.legend(["tach2","tach4"])




plt.figure()
plt.plot(tPlot,tach2)
plt.title("tach2")

plt.figure()
plt.plot(tPlot,tach4)
plt.title("tach4")

def xor(x,y):
    n = len(x)
    a = []
    for i in range(n):
        ans = int(x[i]) ^ int(y[i])
        a.append(ans)
    return a

x = xor(tach2,tach4)

plt.figure()
plt.plot(tPlot,x)
plt.title("XOR")
# %%
