
#%%
import serial
import time
from time import perf_counter as tcounter
import numpy as np
from matplotlib import pyplot as plt
from scipy import fft

#### SETTINGS #########
timeframe = 25 # seconds to record
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
print("logging now!")
time.sleep(3)
while t < timeframe*1e6: # in microseconds
    
    data = arduino.readline()
    Sdata = data.decode().strip().split(",")
    #t, tach2, tach4, pwm, error, cumerror, rpm2, rpm4 = list(map(float,Sdata))
    #print(Sdata)
    try:
        t, reading, theta, rpm, refpwm, sr = list(map(float,Sdata))
    except:
        t, reading, theta, rpm, refpwm, sr = [0] * 6
        print("except!")
    cmd = "29\r"
    
    rawdata.append([t,reading,theta,rpm,refpwm, sr]) #time, pwm, error, cumerror, rpm2, rpm4


rawdata = np.array(rawdata)
#%%

plt.figure()
tplot, reading,theta,rpm, refpwm, sr = [rawdata[:,i] for i in range(6)]
tplot = tplot*1e-6

plt.plot(tplot,theta)
plt.figure()
a,b = [14.95,15.5]
tplotcut = tplot[tplot < b]
rpmcut = rpm[tplot < b]
refcut = refpwm[tplot< b]
rpmcut = rpmcut[tplotcut> a]
refcut = refcut[tplotcut > a]
tplotcut = tplotcut[tplotcut > a]


bias = rpmcut[0]

k = (rpmcut[-1]-bias)/15

plt.figure()
response = (rpmcut-bias)*1.6
u =  k*(refcut-refcut[0])*1.6
tau = tplotcut[response>=.63*u[-1]][0]- tplotcut[0]

plt.plot(tplotcut, response)
plt.plot(tplotcut, u)
#plt.axvline(tplotcut[0]+tau, c = 'red', ls = '--')

plt.xlabel("time [s]")
plt.ylabel("angular velocity [rpm]")
plt.title("Step response")
plt.legend(["Motor response","Input"])
print("K: ", k, "tau: ", tau)

# %%
