@echo off
set ORANGEC=%~dp0
set ORANGEC_USER_DATA=%ORANGEC%appdata
set ORANGEC_PATH_OLD=%PATH%
path %ORANGEC%bin;%PATH%
"%ORANGEC%bin\ocide.exe" %*
set PATH=%ORANGEC_PATH_OLD%
set ORANGEC_PATH_OLD=

