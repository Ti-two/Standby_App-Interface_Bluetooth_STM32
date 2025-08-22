@echo OFF & setlocal ENABLEEXTENSIONS
REM cd ..
SET CURRENTDIR=%CD%
perl\perl.exe %CURRENTDIR%\perl\fusiongcm.pl "app.c" "%CURRENTDIR%" "F105" "L431"
REM exit
