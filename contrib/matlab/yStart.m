function [ handle ] = yStart(workingDir, xmlFile )
%YCONNECT Open the connection to YARS
cmd=sprintf('cd "%s"; yars %s &',workingDir, xmlFile);
[status,cmdout] = system(cmd,'-echo')
disp(status)
disp(cmdout)
%handle = tcpip(hostname, port); 
%set(handle, 'InputBufferSize', 30000); 
%fopen(handle);
handle='a'
end

