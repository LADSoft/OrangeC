@echo off
if "%1"=="/u" goto unset
if "%1"=="/U" goto unset
set DUMMY798="c:\cc386;c:\cc386;%PATH%"
if "%DUMMY798%"==""  %COMSPEC% /k %0 /e:4096
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
