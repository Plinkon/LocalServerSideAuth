@echo off

echo Compiling client and server...
g++ -D_WIN32_WINNT=0x0600 "..\..\src\client\client.cpp" -o "..\..\output\client" -lws2_32
g++ -D_WIN32_WINNT=0x0600 "..\..\src\server\server.cpp" -o "..\..\output\server" -lws2_32

echo Compilation completed.
pause

exit
