function [offsetx,offsety, ratio] = maganalysis(filename)
%% maganalysis graphs and produces calibration data for accelerometer/magnetometer
% filename: .csv of raw output. Rotate magnetometer slowly about z axis to
% find offset values
% output: calibration values for magnetometer

data = load(filename);

mx = data(:,4);
my = data(:,5);

figure();
hold on;
plot(mx);
plot(my);
legend('x','y');

disp('Click the begining and end of rotation data');
[x,y] = ginput(2);
mx = mx(x(1):x(2));
my = my(x(1):x(2));

offsetx = (max(mx) + min(mx))/2;
offsety = (max(my) + min(my))/2;

rx = (max(mx) - min(mx))/2;
ry = (max(my) - min(my))/2;
ratio = ry/rx;

newx1 = (mx - offsetx) * ratio;
newy1 = my-offsety;

figure();
hold on;
scatter(mx,my,'.')
scatter(newx1,newy1,'.')
legend('Raw Data','Calibrated Data');
title('Z axis Rotation, Magnetic Field Components');
xlabel('X (\mu T)'); ylabel('Y (\mu T)');
axis('equal');
grid on

end