@echo off
echo ================================
echo   Please Run As Administator !
echo ================================
echo.
echo Stopping Windows MIDI Service...
net stop midisrv
echo Waiting for the service to fully stop...
timeout /t 3 /nobreak > nul
echo Starting Windows MIDI Service...
net start midisrv
echo Operation completed.
pause