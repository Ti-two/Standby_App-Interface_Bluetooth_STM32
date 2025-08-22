@echo OFF & setlocal ENABLEEXTENSIONS
cd ..
SET CURRENTDIR=%CD%
start /wait  perl\perl.exe %CURRENTDIR%\perl\creategcm.pl "app.c" "%CURRENTDIR%" "Interface_WiFi_Bluetooth" "WIFI BLT F105 Release RampeSolaire" "F105"
start /wait  perl\perl.exe %CURRENTDIR%\perl\fusiongcm.pl "app.c" "%CURRENTDIR%" "F105" "L431"
exit
