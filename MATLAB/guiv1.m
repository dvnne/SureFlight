function varargout = guiv1(varargin)
% GUIV1 MATLAB code for guiv1.fig
%      GUIV1, by itself, creates a new GUIV1 or raises the existing
%      singleton*.
%
%      H = GUIV1 returns the handle to a new GUIV1 or the handle to
%      the existing singleton*.
%
%      GUIV1('CALLBACK',hObject,eventData,handles,...) calls the local
%      function named CALLBACK in GUIV1.M with the given input arguments.
%
%      GUIV1('Property','Value',...) creates a new GUIV1 or raises the
%      existing singleton*.  Starting from the left, property value pairs are
%      applied to the GUI before guiv1_OpeningFcn gets called.  An
%      unrecognized property name or invalid value makes property application
%      stop.  All inputs are passed to guiv1_OpeningFcn via varargin.
%
%      *See GUI Options on GUIDE's Tools menu.  Choose "GUI allows only one
%      instance to run (singleton)".
%
% See also: GUIDE, GUIDATA, GUIHANDLES

% Edit the above text to modify the response to help guiv1

% Last Modified by GUIDE v2.5 06-May-2018 10:09:53

% Begin initialization code - DO NOT EDIT
gui_Singleton = 1;
gui_State = struct('gui_Name',       mfilename, ...
                   'gui_Singleton',  gui_Singleton, ...
                   'gui_OpeningFcn', @guiv1_OpeningFcn, ...
                   'gui_OutputFcn',  @guiv1_OutputFcn, ...
                   'gui_LayoutFcn',  [] , ...
                   'gui_Callback',   []);
if nargin && ischar(varargin{1})
    gui_State.gui_Callback = str2func(varargin{1});
end

if nargout
    [varargout{1:nargout}] = gui_mainfcn(gui_State, varargin{:});
else
    gui_mainfcn(gui_State, varargin{:});
end
% End initialization code - DO NOT EDIT


% --- Executes just before guiv1 is made visible.
function guiv1_OpeningFcn(hObject, eventdata, handles, varargin)
% This function has no output args, see OutputFcn.
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
% varargin   command line arguments to guiv1 (see VARARGIN)

% Choose default command line output for guiv1
handles.output = hObject;

% Update handles structure
guidata(hObject, handles);

if ~isempty(instrfind)
     fclose(instrfind);
      delete(instrfind);
end

fclose(serial('COM3','BAUD', 9600));

s=serial('COM3','BAUD', 9600); % Make sure the baud rate and COM port is 
                              % same as in Arduino IDE
fopen(s);
pause(2.5);

[handles(:).s] = s;

%flag setup
flags.loadon = '5001';
flags.loadoff = '5000';
flags.autoon = '7001';
flags.autooff = '7000';
flags.zeroon = '6001';
flags.zerooff = '6000';
flags.manualdone = '9999';
flags.calerror = '8000';
flags.limerror = '9000';


%ASKING IF THE USER HAS CALIBRATED
answer = questdlg('Have you calibrated the wind station and the pad?', ...
	'Calibration Check','Yes','No', 'No');

switch answer
    case 'Yes'
        handles(:).calibrated = 1;
    case 'No'
        while strcmp(answer, 'No')
            uiwait(warndlg('Please calibrate wind station and pad to continue.','Warning'));
            handles(:).calibrated = 0;
            answer = questdlg('Have you calibrated the wind station and the pad?', ...
            'Calibration Check','Yes','No','No');
        end
end

%update rhe handles struct
handles(:).flags = flags;
guidata(hObject, handles);

% UIWAIT makes guiv1 wait for user response (see UIRESUME)
% uiwait(handles.figure1);


% --- Outputs from this function are returned to the command line.
function varargout = guiv1_OutputFcn(hObject, eventdata, handles) 
% varargout  cell array for returning output args (see VARARGOUT);
% hObject    handle to figure
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Get default command line output from handles structure
varargout{1} = handles.output;


% --- Executes on button press in loadunload.
function loadunload_Callback(hObject, eventdata, handles)
% hObject    handle to loadunload (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%set( findall(handles.your_uipanel, '-property', 'Enable'), 'Enable', 'off')

fprintf(handles.s,handles.flags.loadon);
set(handles.polarinput,'Enable','off')  %disable
set(handles.azimuthinput,'Enable','off')  %disable
set(handles.movePolar,'Enable','off')  %disable move polar button
set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'off')
set(handles.loadunload, 'Enable', 'off')
set(handles.manualmode, 'Enable', 'off')
set(handles.autoangle, 'Enable', 'off')
uiwait(LoadMessage) %makes whole GUI wait until load mode is exited
fprintf(handles.s,handles.flags.loadoff);
set(handles.polarinput,'Enable','on')  %disable
set(handles.azimuthinput,'Enable','on')  %disable
set(handles.movePolar,'Enable','on')  %disable move polar button
set(handles.moveAzimuth,'Enable','on')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'on')
set(handles.loadunload, 'Enable', 'on')
set(handles.manualmode, 'Enable', 'on')
set(handles.autoangle, 'Enable', 'on')


% --- Executes on button press in autoangle.
function autoangle_Callback(hObject, eventdata, handles)
% hObject    handle to autoangle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%show moving status
set(handles.status, 'String', 'MOVING')
set(handles.status, 'BackgroundColor', [1 0.5 0])
%disable evrything while moving
set(handles.polarinput,'Enable','off')  %disable
set(handles.azimuthinput,'Enable','off')  %disable
set(handles.movePolar,'Enable','off')  %disable move polar button
set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'off')
set(handles.loadunload, 'Enable', 'off')
set(handles.manualmode, 'Enable', 'off')
set(handles.autoangle, 'Enable', 'off')
drawnow
fprintf(handles.s,handles.flags.autoon); %tell motors to move to ideal angle
flag = getCommand(handles.s, [str2num(handles.flags.calerror),...
    str2num(handles.flags.limerror), str2num(handles.flags.autooff)]); %waits for command from arduino
%enable evrything done moving
set(handles.polarinput,'Enable','on')  %disable
set(handles.azimuthinput,'Enable','on')  %disable
set(handles.movePolar,'Enable','on')  %disable move polar button
set(handles.moveAzimuth,'Enable','on')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'on')
set(handles.loadunload, 'Enable', 'on')
set(handles.manualmode, 'Enable', 'on')
set(handles.autoangle, 'Enable', 'on')
if flag == 8000
    errordlg('Pad not calibrated!','Calibration Error');
    set(handles.status, 'String', 'NOT READY')
    set(handles.status, 'BackgroundColor', [1 0 0])
    set(handles.manualmode,'Enable','off')  %disable manual mode
    set(handles.autoangle,'Enable','off')  %disable manual mode
elseif flag == 9000
    errordlg('Pad not zeroed! Re-zero and try again.','Zeroing Error');
    set(handles.status, 'String', 'NOT READY')
    set(handles.status, 'BackgroundColor', [1 0 0])
    set(handles.manualmode,'Enable','off')  %disable manual mode
    set(handles.autoangle,'Enable','off')  %disable manual mode
else
    set(handles.status, 'String', 'READY')
    set(handles.status, 'BackgroundColor', [0 1 0])
    set(handles.manualmode,'Enable','on')  %disable manual mode
end


% --- Executes on button press in manualmode.
function manualmode_Callback(hObject, eventdata, handles)
% hObject    handle to manualmode (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hint: get(hObject,'Value') returns toggle state of manualmode
%write new window with user input
if get(handles.manualmode, 'Value')
    set(handles.polarlabel,'Enable','on')  %enable autoangle
    set(handles.polarinput,'Enable','on')  %enable autoangle
    set(handles.azimuthlabel,'Enable','on')  %enable autoangle
    set(handles.azimuthinput,'Enable','on')  %enable autoangle
    set(handles.movePolar,'Enable','on')  %enable move polar button
    set(handles.moveAzimuth,'Enable','on')  %enable move azimtuh button
else
    set(handles.polarlabel,'Enable','off')  %disable
    set(handles.polarinput,'Enable','off')  %disable
    set(handles.azimuthlabel,'Enable','off')  %disable 
    set(handles.azimuthinput,'Enable','off')  %disable
    set(handles.movePolar,'Enable','off')  %disable move polar button
    set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
end



% --- Executes on button press in zeropad.
function zeropad_Callback(hObject, eventdata, handles)
% hObject    handle to zeropad (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
%show moving status
set(handles.status, 'String', 'MOVING')
set(handles.status, 'BackgroundColor', [1 0.5 0])

%disable everything while moving
set(handles.polarinput,'Enable','off')  %disable
set(handles.azimuthinput,'Enable','off')  %disable
set(handles.movePolar,'Enable','off')  %disable move polar button
set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'off')
set(handles.loadunload, 'Enable', 'off')
set(handles.manualmode, 'Enable', 'off')
set(handles.autoangle, 'Enable', 'off')

drawnow;
fprintf(handles.s,handles.flags.zeroon);%tell motors to move to ideal angle
flag = getCommand(handles.s, [str2num(handles.flags.calerror), str2num(handles.flags.zerooff)]); %waits for command from arduino

set(handles.polarinput,'Enable','on')  %disable
set(handles.azimuthinput,'Enable','on')  %disable
set(handles.movePolar,'Enable','on')  %disable move polar button
set(handles.moveAzimuth,'Enable','on')  %disable move azimtuh button
set(handles.zeropad, 'Enable', 'on')
set(handles.loadunload, 'Enable', 'on')
set(handles.manualmode, 'Enable', 'on')
set(handles.autoangle, 'Enable', 'on')

if flag == 8000
    errordlg('Pad not calibrated!','Calibration Error');
    set(handles.status, 'String', 'NOT READY')
    set(handles.status, 'BackgroundColor', [1 0 0])
    set(handles.manualmode,'Value',0)  %disable manual mode
    set(handles.manualmode,'Enable','off')  %disable manual mode
    set(handles.autoangle,'Enable','off')  %disable manual mode
else
    set(handles.status, 'String', 'Zeroed');
    set(handles.status, 'BackgroundColor', [.12 .56 1]);
    set(handles.manualmode,'Enable','on')  %enable manual mode
    set(handles.autoangle,'Enable','on')  %enable manual mode
end





function polarinput_Callback(hObject, eventdata, handles)
% hObject    handle to polarinput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of polarinput as text
%        str2double(get(hObject,'String')) returns contents of polarinput as a double


% --- Executes during object creation, after setting all properties.
function polarinput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to polarinput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end



function azimuthinput_Callback(hObject, eventdata, handles)
% hObject    handle to azimuthinput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

% Hints: get(hObject,'String') returns contents of azimuthinput as text
%        str2double(get(hObject,'String')) returns contents of azimuthinput as a double



% --- Executes during object creation, after setting all properties.
function azimuthinput_CreateFcn(hObject, eventdata, handles)
% hObject    handle to azimuthinput (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    empty - handles not created until after all CreateFcns called

% Hint: edit controls usually have a white background on Windows.
%       See ISPC and COMPUTER.
if ispc && isequal(get(hObject,'BackgroundColor'), get(0,'defaultUicontrolBackgroundColor'))
    set(hObject,'BackgroundColor','white');
end


% --- Executes on button press in movePolar.
function movePolar_Callback(hObject, eventdata, handles)
% hObject    handle to movePolar (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

[handles(:).polar] = get(handles.polarinput,'String'); %get data from text input
%check if data in valid range and integer
if (0 <= str2num(handles.polar)) && (str2num(handles.polar) <= 360) && (rem(str2num(handles.polar), 1) ==0)
    polarsend = str2num(handles.polar) +100;
    cmd = strcat(num2str(polarsend), '1');
   
    set(handles.status, 'String', 'MOVING')
    set(handles.status, 'BackgroundColor', [1 0.5 0])    
    %disable everything while moving
    set(handles.polarinput,'Enable','off')  %disable
    set(handles.azimuthinput,'Enable','off')  %disable
    set(handles.movePolar,'Enable','off')  %disable move polar button
    set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
    set(handles.zeropad, 'Enable', 'off')
    set(handles.loadunload, 'Enable', 'off')
    set(handles.manualmode, 'Enable', 'off')
    set(handles.autoangle, 'Enable', 'off') 
    drawnow
    fprintf(handles.s,cmd);%tell motors to move to ideal angle
    flag = getCommand(handles.s, [str2num(handles.flags.calerror),...
    str2num(handles.flags.limerror), str2num(handles.flags.manualdone)]); %waits for command from arduino
    
    set(handles.polarinput,'Enable','on')  %disable
    set(handles.azimuthinput,'Enable','on')  %disable
    set(handles.movePolar,'Enable','on')  %disable move polar button
    set(handles.moveAzimuth,'Enable','on')  %disable move azimtuh button
    set(handles.zeropad, 'Enable', 'on')
    set(handles.loadunload, 'Enable', 'on')
    set(handles.manualmode, 'Enable', 'on')
    set(handles.autoangle, 'Enable', 'on')
    if flag == 8000
        errordlg('Pad not calibrated!','Calibration Error');
        set(handles.status, 'String', 'NOT READY')
        set(handles.status, 'BackgroundColor', [1 0 0])
        % We turn back on the interface
        set(InterfaceObj,'Enable','on')
        set(handles.manualmode,'Value',0)  %disable manual mode
        set(handles.manualmode,'Enable','off')  %disable manual mode
        set(handles.autoangle,'Enable','off')  %disable auto mode
    elseif flag == 9000
        errordlg('Limit switch hit! Please re-zero pad.','Limit Switch Error');
        set(handles.status, 'String', 'NOT READY')
        set(handles.status, 'BackgroundColor', [1 0 0])
        % We turn back on the interface
        set(InterfaceObj,'Enable','on')
        set(handles.manualmode,'Value',0)  %disable manual mode
        set(handles.manualmode,'Enable','off')  %disable manual mode
        set(handles.autoangle,'Enable','off')  %disable manual mode
    else
        set(handles.status, 'String', 'READY');
        set(handles.status, 'BackgroundColor', [0 1 0]);
          % We turn back on the interface
        %set(InterfaceObj,'Enable','on')
    end   
else
    f = errordlg('Polar input angle must be an integer between 0 and 360','RangeError');
    uiwait(f)
end
guidata(hObject, handles);



% --- Executes on button press in moveAzimuth.
function moveAzimuth_Callback(hObject, eventdata, handles)
% hObject    handle to moveAzimuth (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
[handles(:).azimuth] = get(handles.azimuthinput,'String');
    
%check if input angle is an integer between 0 and 20
if (0<= str2num(handles.azimuth)) && (str2num(handles.azimuth) <= 20) && (rem(str2num(handles.azimuth), 1) ==0)
    azimuthsend = str2num(handles.azimuth)+100;
    cmd = strcat(num2str(azimuthsend), '0');
    set(handles.status, 'String', 'Moving')
    set(handles.status, 'BackgroundColor', [1 0.5 0])
    %disable everything while moving
    set(handles.polarinput,'Enable','off')  %disable
    set(handles.azimuthinput,'Enable','off')  %disable
    set(handles.movePolar,'Enable','off')  %disable move polar button
    set(handles.moveAzimuth,'Enable','off')  %disable move azimtuh button
    set(handles.zeropad, 'Enable', 'off')
    set(handles.loadunload, 'Enable', 'off')
    set(handles.manualmode, 'Enable', 'off')
    set(handles.autoangle, 'Enable', 'off')
    
    drawnow
    fprintf(handles.s,cmd);%tell motors to move to ideal angle
        flag = getCommand(handles.s, [str2num(handles.flags.calerror),...
    str2num(handles.flags.limerror), str2num(handles.flags.manualdone)]); %waits for command from arduino

    set(handles.polarinput,'Enable','on')  %disable
    set(handles.azimuthinput,'Enable','on')  %disable
    set(handles.movePolar,'Enable','on')  %disable move polar button
    set(handles.moveAzimuth,'Enable','on')  %disable move azimtuh button
    set(handles.zeropad, 'Enable', 'on')
    set(handles.loadunload, 'Enable', 'on')
    set(handles.manualmode, 'Enable', 'on')
    set(handles.autoangle, 'Enable', 'on')

    if flag == 8000
        errordlg('Pad not calibrated!','Calibration Error');
        set(handles.status, 'String', 'NOT READY')
        set(handles.status, 'BackgroundColor', [1 0 0])
        % We turn back on the interface
        %set(InterfaceObj,'Enable','on')
        set(handles.manualmode,'Value',0)  %disable manual mode
        set(handles.manualmode,'Enable','off')  %disable manual mode
        set(handles.autoangle,'Enable','off')  %disable auto mode
    elseif flag == 9000
        errordlg('Limit switch hit! Please re-zero pad.','Limit Switch Error');
        set(handles.status, 'String', 'NOT READY')
        set(handles.status, 'BackgroundColor', [1 0 0])
        set(handles.manualmode,'Value',0)  %disable manual mode
        set(handles.manualmode,'Enable','off')  %disable manual mode
        set(handles.autoangle,'Enable','off')  %disable manual mode
    else
        set(handles.status, 'String', 'READY');
        set(handles.status, 'BackgroundColor', [0 1 0]);
    end 
   
else
    f = errordlg('Azimuth input angle must be an integer between 0 and 20','RangeError');
    uiwait(f)
end
