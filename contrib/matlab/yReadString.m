function [ s ] = yReadString( handle )
%YREADSTRING Summary of this function goes here
%   Detailed explanation goes here

type = fread(handle, 1);

if type ~= 's'
    sprintf('Error in yReadString. Expected s but received %c',type)
end

data = fread(handle, 4);
n = yIntOfData(data);
s = fread(handle, n);
s=strcat(reshape(s,1,[]));

end