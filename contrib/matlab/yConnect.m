function [ handle ] = yConnect( hostname, port )
%YCONNECT Open the connection to YARS
handle = tcpip(hostname, port); 
set(handle, 'InputBufferSize', 30000); 
fopen(handle);
end