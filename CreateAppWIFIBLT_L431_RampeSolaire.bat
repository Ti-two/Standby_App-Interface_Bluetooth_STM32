@echo OFF & setlocal ENABLEEXTENSIONS
cd ..
SET CURRENTDIR=%CD%
del "%CURRENTDIR%\WIFI BLT L431 Release RampeSolaire\Interface_WiFi_Bluetooth.hex"
perl\perl.exe %CURRENTDIR%\perl\createapp.pl "app.c" "%CURRENTDIR%"
REM exit