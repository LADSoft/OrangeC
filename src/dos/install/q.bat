@echo off
if "%1"=="/u" goto unset
if "%1"=="/U" goto unset
if "%LADSOFT%"=="" set LADSOFT=C:\CC386
set PATHBAK=%PATH%
if not "%PATHBAK%"=="%PATH%" %COMSPEC% /k %0 /e:4096
path C:\CC386\BIN;%PATH%
if not "%PATH%"=="C:\CC386\BIN;%PATHBAK%" %COMSPEC% /k %0 /e:4096
set PROMPTBAK=%PROMPT%
if not "%PROMPTBAK%"=="%PROMPT%" %COMSPEC% /k %0 /e:4096
set PROMPT=[ %PROMPT% - CC386 ]
if not "%PROMPT%"=="[ %PROMPTBAK% - CC386 ]" %COMSPEC% /k %0 /e:4096
goto end
:unset
if "%LADSOFT%"=="" goto end
path %PATHBAK%
prompt %PROMPTBAK%
set LADSOFT=
set PATHBAK=
set PROMPTBAK=
:end 