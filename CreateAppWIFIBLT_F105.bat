@echo OFF & setlocal ENABLEEXTENSIONS
cd ..
SET CURRENTDIR=%CD%
del "%CURRENTDIR%\WIFI BLT F105 Release\Interface_WiFi_Bluetooth.hex"
perl\perl.exe %CURRENTDIR%\perl\createapp.pl "app.c" "%CURRENTDIR%"
REM exit