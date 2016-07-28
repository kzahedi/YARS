function [ i ] = yIntOfData( b )
%YINTOFDATA Summary of this function goes here
%   Detailed explanation goes here

i = typecast(b,'int32');
i0 = bitshift(b(1),  0);
i1 = bitshift(b(2),  8);
i2 = bitshift(b(3), 16);
i3 = bitshift(b(4), 24);

i = i0 + i1 + i2 + i3;

end