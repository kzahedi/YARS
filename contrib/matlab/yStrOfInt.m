function [ s ] = yStrOfInt( i )
%YSTROFINT Summary of this function goes here
%   Detailed explanation goes here

mask = hex2dec('000000ff');

i0 = bitshift(i,  0);
i1 = bitshift(i, -8);
i2 = bitshift(i,-16);
i3 = bitshift(i,-24);

byte0 = uint8(bitand(i0,mask));
byte1 = uint8(bitand(i1,mask));
byte2 = uint8(bitand(i2,mask));
byte3 = uint8(bitand(i3,mask));

s=sprintf('%c%c%c%c',byte0,byte1,byte2,byte3);

end