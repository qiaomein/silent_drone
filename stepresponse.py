
#%%
import serial
import time
from time import perf_counter as tcounter
import numpy as np
from matplotlib import pyplot as plt
from scipy import fft

#### SETTINGS #########
timeframe = 50 # seconds to record
port_name = "COM7"
baudrate = 2000000
interval = 10

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
flag = True

while t < timeframe*1e6: # in microseconds
    
    data = arduino.readline()
    Sdata = data.decode().strip().split(",")
    #t, tach2, tach4, pwm, error, cumerror, rpm2, rpm4 = list(map(float,Sdata))
    #print(Sdata)
    try:
        t, reading, theta, rpm, sr = list(map(float,Sdata))
    except:
        t, reading, theta, rpm, sr = [0] * 5
        print("except!")
    cmd = "29\r"
    
    if trigger(t) and flag:
        print(t, cmd)
        arduino.write(cmd.encode())
        flag = False
    rawdata.append([t,reading,theta,rpm,sr]) #time, pwm, error, cumerror, rpm2, rpm4
    
    #print(t)

arduino.write("-29\r".encode())
rawdata = np.array(rawdata)
#%%

plt.figure()
tplot, reading,theta,rpm,sr = [rawdata[:,i] for i in range(5)]
plt.plot(tplot*1e-6,theta)
plt.figure()
plt.plot(tplot*1e-6,rpm)

# %%
