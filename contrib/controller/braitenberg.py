import math

def init():
  print "init called"

def update(sensors):
  sleft  = 1.0/3.0 * (sensors[0] + sensors[1] + sensors[2])
  sright = 1.0/3.0 * (sensors[3] + sensors[4] + sensors[5])
  actuators = [ math.tanh(-sright), math.tanh(-sleft)]
  return actuators

def reset():
  print "reset called"

def close():
  print "close called"
