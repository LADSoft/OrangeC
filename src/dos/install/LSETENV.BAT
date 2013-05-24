@echo off
REM
REM Beginning of LADSoft CC386 installation
REM
if "%1"=="/u" goto unset
if "%1"=="/U" goto unset
set DUMMY798=C:\CC386;C:\CC386\bin;%PATH%1234567890
if "%DUMMY798%"==""  %COMSPEC% /k %0 /e:4096
set DUMMY798=
set LADSOFT=C:\CC386
set PATHBAK=%PATH%
path C:\CC386\BIN;%PATH%
goto end
:unset
if "%LADSOFT%"=="" goto end
path %PATHBAK%
set LADSOFT=
set PATHBAK=
:end
REM
REM End of LADSoft CC386 installation
REM
