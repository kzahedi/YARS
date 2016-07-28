function [] = yClose( handle )
%YCLOSE Close the connection to YARS
fclose(handle); 
delete(handle); 
clear handle
end