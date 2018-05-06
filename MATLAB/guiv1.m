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

    set(handles.polarlabel,'Enable','on')  %enable autoangle
    set(handles.polarinput,'Enable','on')  %enable autoangle
    set(handles.azimuthlabel,'Enable','on')  %enable autoangle
    set(handles.azimuthinput,'Enable','on')  %enable autoangle
    set(handles.movePolar,'Enable','on')  %enable move polar button
    set(handles.moveAzimuth,'Enable','on')  %enable move azimtuh button

%flag setup
[handles(:).flags(:).loadon] = '4001';
[handles(:).flags(:).loadoff] = '4000';
[handles(:).flags(:).autoon] = '7001';
[handles(:).flags(:).autooff] = '7000';
[handles(:).flags(:).zeroon] = '6001';
[handles(:).flags(:).zerooff] = '6000';
[handles(:).flags(:).manualdone] = '9999';


%ASK IF THEY HAVE CALIBRATED
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
uiwait(LoadMessage) %makes whole GUI wait until load mode is exited
fprintf(handles.s,handles.flags.loadoff);

% --- Executes on button press in autoangle.
function autoangle_Callback(hObject, eventdata, handles)
% hObject    handle to autoangle (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)

%show moving status
set(handles.status, 'String', 'Moving')
set(handles.status, 'BackgroundColor', [1 0.5 0])
fprintf(handles.s,handles.flags.autoon); %tell motors to move to ideal angle
getCommand(handles.s, str2num(handles.flags.autooff)); %waits for command from arduino
%show status as ready
set(handles.status, 'String', 'Ready')
set(handles.status, 'BackgroundColor', [0 1 0])
set(handles.manualmode,'Enable','on')  %enable manual mode


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
    set(handles.polarlabel,'Enable','off')  %enable autoangle
    set(handles.polarinput,'Enable','off')  %enable autoangle
    set(handles.azimuthlabel,'Enable','off')  %enable autoangle
    set(handles.azimuthinput,'Enable','off')  %enable autoangle
    set(handles.movePolar,'Enable','off')  %enable move polar button
    set(handles.moveAzimuth,'Enable','off')  %enable move azimtuh button
end



% --- Executes on button press in zeropad.
function zeropad_Callback(hObject, eventdata, handles)
% hObject    handle to zeropad (see GCBO)
% eventdata  reserved - to be defined in a future version of MATLAB
% handles    structure with handles and user data (see GUIDATA)
s%show moving status
set(handles.status, 'String', 'Moving')
set(handles.status, 'BackgroundColor', [1 0.5 0])
fprintf(handles.s,handles.flags.zeroon);%tell motors to move to ideal angle
getCommand(handles.s, str2num(handles.flags.zerooff));%wait for message from arduino
%show status as zeroed
set(handles.status, 'String', 'Zeroed');
set(handles.status, 'BackgroundColor', [0 0 1]);




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
%check if datat in valid range and integer
if (0 <= str2num(handles.polar)) && (str2num(handles.polar) <= 360) && (rem(str2num(handles.polar), 1) ==0)
    cmd = strcat(handles.polar, '1');
   
    set(handles.status, 'String', 'Moving')
    set(handles.status, 'BackgroundColor', [1 0.5 0])
    %disable the gui while moving
    InterfaceObj=findobj(guiv1,'Enable','on');
    set(InterfaceObj,'Enable','off');
    set(handles.status, 'Enable', 'on')
 
    fprintf(handles.s,cmd);%tell motors to move to ideal angle
    getCommand(handles.s, str2num(handles.flags.manualdone))
    
    set(handles.status, 'String', 'Ready')
    set(handles.status, 'BackgroundColor', [0 1 0])
    % We turn back on the interface
    set(InterfaceObj,'Enable','on')
    
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

if (0<= handles.azimuth) && (handles.azimuth <= 20) && (rem(handles.azimuth, 1) ==0)
    cmd = str2num(strcat(handles.azimuth, '0'));
    set(handles.status, 'String', 'Moving')
    set(handles.status, 'BackgroundColor', [1 0.5 0])
    InterfaceObj=findobj(guiv1,'Enable','on');
    set(InterfaceObj,'Enable','off');
    set(handles.status, 'Enable', 'on')
 
    fprintf(handles.s,cmd);%tell motors to move to ideal angle
    getCommand(9999, handles.s);
%     flag = str2double(fgetl(handles.s));
%     %keep reading until the start data is returned
%     while flag ~= 9999
%      % pause
%      pause(.01); 
%      % try again
%      flag = str2double(fgetl(handles.s));
%     end

    set(handles.status, 'String', 'Ready')
    set(handles.status, 'BackgroundColor', [0 1 0])
    % We turn back on the interface
    set(InterfaceObj,'Enable','on')
    %end
    set(handles.status, 'String', 'Ready')
    set(handles.status, 'BackgroundColor', [0 1 0])
    set(handles.manualmode,'Enable','on')  %enable manual mode
else
    f = errordlg('Azimuth input angle must be an integer between 0 and 20','RangeError');
    uiwait(f)
end
