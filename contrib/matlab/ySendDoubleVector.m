function [  ] = ySendDoubleVector( handle, v )
%YSENDDOUBLEVECTOR Summary of this function goes here
%   Detailed explanation goes here

  is = yStrOfInt(length(v));
  %ds = strcat(typecast(double(v),'uint8'));
  w = typecast(double(v),'uint8');
  ds = '';
  for i = 1:length(w)
    ds = sprintf('%s%s',ds,w(i));
  end
  cmd = sprintf('D%s%s',is,ds);
  fwrite(handle, cmd);
end
