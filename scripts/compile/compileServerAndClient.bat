@echo off
REM Get the names of the client and server files from user input and compile them

echo Enter the name of the client file (e.g. client.cpp):
set /p clientFile=
echo Enter the name of the server file (e.g. server.cpp):
set /p serverFile=

REM If the client file does not end with .cpp, append it
if /I not "%clientFile:~-4%"==".cpp" (
    set clientFile=%clientFile%.cpp
)

REM If the server file does not end with .cpp, append it
if /I not "%serverFile:~-4%"==".cpp" (
    set serverFile=%serverFile%.cpp
)

echo Compiling %clientFile% and %serverFile%... 
g++ -D_WIN32_WINNT=0x0600 "..\..\src\client\%clientFile%" -o "..\..\output\client" -lws2_32
g++ -D_WIN32_WINNT=0x0600 "..\..\src\server\%serverFile%" -o "..\..\output\server" -lws2_32

echo Compilation completed.
pause
