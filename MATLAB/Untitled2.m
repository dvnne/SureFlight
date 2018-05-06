if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end

fclose(serial('COM3','BAUD', 9600));

s=serial('COM3','BAUD', 9600); % Make sure the baud rate and COM port is 
                              % same as in Arduino IDE
fopen(s);
pause(3)

flag = str2double(fgetl(s));