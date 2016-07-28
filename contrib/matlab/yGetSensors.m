function [ data ] = yGetSensors( handle )
%YGETSENSORS Summary of this function goes here
%   Detailed explanation goes here


ySendString(handle, 'SENSORS');
data = yReadDoubleVector(handle);

end

