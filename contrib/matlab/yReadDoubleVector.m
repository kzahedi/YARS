function [ data ] = yReadDoubleVector( handle )
%YREADDOUBLEVECTOR Summary of this function goes here
%   Detailed explanation goes here

  type = fread(handle, 1);

  if type ~= 'D'
      sprintf('Error in yReadDoubleVector. Expected D but received %c', type)
  end

  data = fread(handle, 4);
  n = yIntOfData(data);
  b = fread(handle, n*8);

  data = zeros(1,n);

  for i=1:n
      d = reshape(b((i-1)*8+1:(i-1)*8+8),1,[]);
      data(i) = typecast(uint8(d),'double');
  end
end
