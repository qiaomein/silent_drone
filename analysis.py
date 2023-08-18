import serial
import time
from time import perf_counter as tcounter
import numpy as np
from matplotlib import pyplot as plt
from scipy import fft

def load(filename):
    return np.loadtxt(filename)

def xor(x, y):
    n = len(x)
    a = []
    for i in range(n):
        ans = int(x[i]) ^ int(y[i])
        a.append(ans)
    return a


def close():
    plt.close("all")
    
    
filename = "1_4000_pidcontrol_mid.csv" ################

rawdata = load(filename)


tp, tach1, tach2, pwm2, rpm1, rpm2, sr, error= [rawdata[:,i] for i in range(8)]
tplot = (tp - tp[0])*1e-6
"""
for x in np.arange(15,20,.2):
    plt.figure()

    a,b = [x,x+.2]
    tplotcut = tplot[tplot < b]
    tach1cut = tach1[tplot < b]
    tach2cut = tach2[tplot< b]
    tach1cut = tach1cut[tplotcut> a]
    tach2cut = tach2cut[tplotcut > a]
    tplotcut = tplotcut[tplotcut > a]


    plt.plot(tplotcut,tach1cut, tplotcut,tach2cut)
    plt.xlabel("time [s]")
    plt.ylabel("tach readings")
    plt.legend(["tach1","tach2"])
"""
    
#%%

plt.figure()
x = 0
a,b = [x,x+10]
tplotcut = tplot[tplot < b]
tach1cut = tach1[tplot < b]
tach2cut = tach2[tplot< b]
tach1cut = tach1cut[tplotcut> a]
tach2cut = tach2cut[tplotcut > a]
tplotcut = tplotcut[tplotcut > a]


plt.plot(tplotcut,tach1cut, tplotcut,tach2cut)
plt.xlabel("time [s]")
plt.ylabel("tach readings")
plt.legend(["tach1","tach2"])

plt.figure()
plt.plot(tplot,error)
plt.ylim([-90,90])
# %%

