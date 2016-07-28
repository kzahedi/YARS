function [  ] = ySendMotorCommands( handle, v )
%YSENDMOTORCOMMANDS Summary of this function goes here
%   Detailed explanation goes here

ySendString(handle,'ACTUATORS');
ySendDoubleVector(handle,v);

end

