@echo off
setlocal enabledelayedexpansion

REM --- CONFIGURATION ---
REM Define the port range to scan. Adjust these values as needed.
set "startPort=1024"
set "endPort=1428"

REM Define the IP address to scan. (Typically localhost is 127.0.0.1)
set "ipAddress=127.0.0.1"

echo Scanning for available ports on %ipAddress% from %startPort% to %endPort%...
echo.

REM --- STEP 1: Get list of used ports on the specified IP ---
REM Capture all active connections on %ipAddress% into a temporary file.
netstat -an | findstr "%ipAddress%" > usedPorts.txt

REM --- STEP 2: Scan the defined range for available ports ---
set "availablePorts="

for /L %%P in (%startPort%,1,%endPort%) do (
    REM Check if the current port is present in the used ports file.
    findstr /C:":%%P " usedPorts.txt >nul
    if errorlevel 1 (
        echo Port %%P is available.
        set "availablePorts=!availablePorts! %%P"
    )
)

REM Clean up the temporary file.
del usedPorts.txt

if "!availablePorts!"=="" (
    echo No available ports found in the specified range.
    pause
    exit /b 1
)

echo.
echo Available ports: !availablePorts!
echo.

REM --- STEP 3: Prompt the user to choose a port or type "random" ---
set /p chosenPort="Enter the port you want to use from the above list (or type 'random' for a random selection): "

if /I "%chosenPort%"=="random" (
    REM Count the number of available ports.
    set "count=0"
    for %%A in (!availablePorts!) do (
         set /A count+=1
    )
    if !count! equ 0 (
         echo No available ports found.
         pause
         exit /b 1
    )
    REM Generate a random index between 1 and count.
    set /A randIndex=%RANDOM% %% !count! + 1
    set "index=0"
    for %%A in (!availablePorts!) do (
         set /A index+=1
         if !index! equ !randIndex! (
             set "chosenPort=%%A"
         )
    )
    echo Random port chosen: !chosenPort!
)

REM --- STEP 4: Save the chosen port to the specified file ---
echo %chosenPort% > ..\..\src\port\port.txt
echo Port %chosenPort% saved to ..\..\src\port\port.txt

pause
exit /b 0