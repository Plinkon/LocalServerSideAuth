@echo off
REM Get the names of the client and server files from user input and compile them

echo Enter the name of the server file (e.g. server.cpp):
set /p serverFile=

REM If the server file does not end with .cpp, append it
if /I not "%serverFile:~-4%"==".cpp" (
    set serverFile=%serverFile%.cpp
)

echo Compiling %serverFile%...
g++ -D_WIN32_WINNT=0x0600 "..\..\src\server\%serverFile%" -o "..\..\output\server" -lws2_32

echo Compilation completed.
pause
