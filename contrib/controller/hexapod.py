import math
import numpy

t = 0;

def init():
  pass

def close():
  pass

def update(sensors):
  global t

  #  plt.clf()
  #  plt.plot(sensors)
  #  plt.ylabel('some numbers')
  #  plt.show()

  t = t + 1

  s1 = math.sin(t * 2.0 * math.pi/50.0);
  s2 = math.sin(t * 2.0 * math.pi/50.0 + 3.0 * math.pi/2.0);
  s3 = math.sin(t * 2.0 * math.pi/50.0 +       math.pi);
  s4 = math.sin(t * 2.0 * math.pi/50.0 + 5.0 * math.pi/2.0);

  a1  = s1
  a2  = s2
  a3  = s3
  a4  = s4

  a5  = s3
  a6  = s4
  a7  = s1
  a8  = s2

  a9  = s1
  a10 = s2
  a11 = s3
  a12 = s4

  # a = numpy.random.rand(1,12) * 2.0 - 1.0
  # a = a.tolist()[0]

  a = [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12]

  return a
