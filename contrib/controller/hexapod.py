import math

t = 0;

def init():
  pass

def close():
  pass

def update(sensors):
  global t

  t = t + 1

  a1  = math.sin(                t/10.0)
  a2  = math.sin(1.5 * math.pi + t/10.0)
  a3  = math.sin(      math.pi + t/10.0)
  a4  = math.sin(0.5 * math.pi + t/10.0)

  a5  = math.sin(      math.pi + t/10.0)
  a6  = math.sin(0.5 * math.pi + t/10.0)
  a7  = math.sin(                t/10.0)
  a8  = math.sin(1.5 * math.pi + t/10.0)

  a9  = math.sin(                t/10.0)
  a10 = math.sin(1.5 * math.pi + t/10.0)
  a11 = math.sin(      math.pi + t/10.0)
  a12 = math.sin(0.5 * math.pi + t/10.0)

  return [a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a12]
