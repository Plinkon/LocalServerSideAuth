@echo off
REM Get the names of the client and server files from user input and compile them

echo Enter the name of the client file (e.g. client.cpp):
set /p clientFile=

REM If the client file does not end with .cpp, append it
if /I not "%clientFile:~-4%"==".cpp" (
    set clientFile=%clientFile%.cpp
)

echo Compiling %clientFile%...
g++ -D_WIN32_WINNT=0x0600 "..\..\src\client\%clientFile%" -o "..\..\output\client" -lws2_32

echo Compilation completed.
pause
