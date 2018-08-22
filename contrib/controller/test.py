nrOfActuators = 12
t = 0
v = 1.0

def init():
  pass

def close():
  pass

def update(sensors):
  global t

  t = t + 1

  print("Sensors: ", s)
  a = []
  for i in range(0, nrOfActuators):
      a.append(v)
  print(a)

  return a
