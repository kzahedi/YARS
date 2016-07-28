function [ ] = ySendString( handle, string )
%YSENDSTRING Summary of this function goes here
%   Detailed explanation goes here

is = yStrOfInt(length(string));
cmd = sprintf('s%s%s',is,string);
fwrite(handle, cmd);
end