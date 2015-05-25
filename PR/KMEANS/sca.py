import matplotlib.pyplot
import pylab
import matplotlib.cm as cm
import numpy as np

k=int(raw_input())
px=[]
py=[]
pz=[]


for x in xrange(0,k):
	print x
	px.append([])
	py.append([])
	pz.append([])

for x in xrange(0,600):
	[x,y,z]=map(float,raw_input().split())
	#print x,y,z
	temp=int(z)
	#print temp
	#exit(0)
	px[temp].append(x)
	py[temp].append(y)

colors = iter(cm.rainbow(np.linspace(0, 1, k+1)))

for x in xrange(0,k):
	#if px[x]:
	matplotlib.pyplot.scatter(px[x],py[x],color=next(colors))

matplotlib.pyplot.show()