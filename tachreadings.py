# %%
import serial
import time
from time import perf_counter as tcounter
import numpy as np
from matplotlib import pyplot as plt
from scipy import fft


def xor(x, y):
    n = len(x)
    a = []
    for i in range(n):
        ans = int(x[i]) ^ int(y[i])
        a.append(ans)
    return a


#### SETTINGS #########
timeframe = 50  # seconds to record
port_name = "COM7"
baudrate = 2000000
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

    ts = t_micro / (1e6)
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
print("LOGGING!")

while t < timeframe * 1e6:  # in microseconds

    data = arduino.readline()
    Sdata = data.decode().strip().split(",")
    try:
        t, tach1, tach2, pwm2, rpm1, rpm2, sr, error = list(map(float, Sdata))
    except:
        t, tach1, tach2, pwm2, rpm1, rpm2, sr, error = [0] * 8
        print("except!")

    rawdata.append([t, tach1, tach2, pwm2, rpm1, rpm2, sr, error])  # time, pwm, error, cumerror, rpm2, rpm4

rawdata = np.array(rawdata)
print("DONE")

tplot, tach1, tach2, pwm2, rpm1, rpm2, sr, error= [rawdata[:,i] for i in range(8)]
tplot = tplot - tplot[0]
# %%


plt.figure()
a,b = [0,20]
tplotcut = tplot[tplot < b]
tach1cut = tach1[tplot < b]
tach2cut = tach2[tplot< b]
tach1cut = tach1cut[tplotcut> a]
tach2cut = tach2cut[tplotcut > a]
tplotcut = tplotcut[tplotcut > a]


plt.plot(tplotcut, tach1cut, tplotcut, tach2cut)
plt.xlabel("time [s]")
plt.ylabel("tach readings")
plt.legend(["tach1", "tach2"])

