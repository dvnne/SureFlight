function [ output_args ] = getCommand(serialobj, codenum )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
    flag = str2double(fgetl(serialobj));
    %keep reading until the start data is returned
    while flag ~= codenum
     % pause
     pause(.01); 
     % try again
     flag = str2double(fgetl(serialobj));
    end

end

