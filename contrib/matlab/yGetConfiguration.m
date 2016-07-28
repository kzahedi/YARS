function [ cfg ] = yGetConfiguration( handle )
%YGETCONFIGURATION Summary of this function goes here
%   Detailed explanation goes here

ySendString(handle,'CONFIGURATION');

s = yReadString(handle);

str = '';

while strcmp(s,'END CONFIGURATION') ~= 1
    % TODO: store configuration in data structure
    s = yReadString(handle);
    str = sprintf('%s\n%s',str,s);
end

disp(str)

end

