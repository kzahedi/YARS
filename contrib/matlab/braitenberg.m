% 1. Verbindung mit YARS herstellen
robot = yStart('/Users/zahedi/Dropbox/yars - Systems Design', 'xml/tcpip_braitenberg.xml');

robot = yConnect('localhost', 9504)


yGetConfiguration(robot)
 
for i=1:2
    for j=1:300
        sensors = yGetSensors(robot);
        left  = -(sensors(1) + sensors(2) + sensors(3)) / 3.0;
        right = -(sensors(4) + sensors(5) + sensors(6)) / 3.0;
        ySendMotorCommands(robot, [right, left]);
    end
    yReset(robot);
end
yQuit(robot);
yClose(robot);
