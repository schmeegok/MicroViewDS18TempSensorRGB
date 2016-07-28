# -*- coding: utf-8 -*-
"""
Spyder Editor


"""

from matplotlib import pyplot as plt
from matplotlib import pylab
from matplotlib.pylab import *
import numpy
from numpy import *

x0 = arange(0.0,31.9,0.1)
x1 = arange(32.0,70.0,0.1)
x2 = arange(70.1,90.0,0.1)

t = x0.tolist() + x1.tolist() + x2.tolist()

print len(x1), len(x2)
#x = x1.extend(x2)


# Slopes
mr1 = (0.00-255.00)/(32.00-0.00)
mb1 = 0
mb2 = (0.00-255.00)/(70.00-32.00)
mg1 = (255.00-0.00)/(70.00-32.00)
mg2 = (0.00-255.00)/(90.00-70.00)
mr2 = (255.00-0.00)/(90.00-70.00)

y0 = []
y02 = []
y1 = []
y2 = []
y3 = []
y4 = []
y5 = []
y6 = []

for i in range(0,len(t)):
    y0.append(mr1*t[i] + (256 - mr1*t[t.index(0.0)]))
    y02.append(256)
    y1.append(mb2*t[i] + (256.00 - mb2*t[t.index(32.0)]))
    y2.append(mg1*t[i] + (0.0 - mg1*t[t.index(32.0)]))
    y3.append(mg2*t[i] + (256.00 - mg2*t[t.index(70.1)]))
    y4.append(mr2*t[i] +(0.0 - mr2*t[t.index(70.1)]))
    y5.append(0)
    y6.append(255)

plt.figure()
plt.plot(t,y0,'r',
         t,y02,'b',
         t,y1,'b',
         t,y2,'g',
         t,y3,'g',
         t,y4,'r',
         #t,y5,'c--',
         #t,y6,'c--'
         )
plt.xlabel("Temperature Degrees F")
plt.ylabel("Arduino PWM Value")
plt.ylim([0,260])
plt.grid()

plt.show()
