@echo off
     set PARALLEL=4
     if "%TRAVIS_OS_NAME%" NEQ "" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
     )
     if "%ORANGEC_HOME%" NEQ "" (
         del /Q ..\bin\*.*
         del /Q ..\lib\*.*
         del /Q ..\include\*.*
         del /Q ..\include\win32\*.*
         del /Q ..\include\sys\*.*
         omake -DCOMPILER=MS clean -j:8
         omake -DCOMPILER=CLANG clean -j:8
         omake -DCOMPILER=MINGW64 clean -j:8
         set BUILD_PROFILE=MS
         set PARALLEL=8
     )
              cd c:\orangec\src
              echo WScript.Echo(Math.floor(new Date().getTime()/1000)); > %temp%\time.js
              for /f %%i in ('cscript //nologo %temp%\time.js') do set SOURCE_DATE_EPOCH=%%i
              del %temp%\time.js
              copy omake.exe \orangec\temp
     if "%ORANGEC_HOME%" EQU "" (
              call c:\orangec\appveyorversion.bat
     )
              IF "%BUILD_PROFILE%" EQU "OCCIL" goto occil
              IF "%BUILD_PROFILE%" EQU "MSDEBUGBUILD" goto msdebugbuild
              IF "%BUILD_PROFILE%" EQU "CODEANALYZER" goto codeanalyzer
              IF "%BUILD_PROFILE%" NEQ "TEST" goto normal
              goto error
:occil
                  REM  alternate build with OCCIL
                  c:\orangec\temp\omake /DCOMPILER=CLANG fullbuild -j:%PARALLEL%
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:%PARALLEL%
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:%PARALLEL%
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\tests
                  omake -B /Coccil /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  echo succeeded
                  goto done
:msdebugbuild
                  REM  Build with Microsoft PDB files
                  c:\orangec\temp\omake /DCOMPILER=MS /DMSPDB=%MSPDB% fullbuild
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  echo succeeded
                  goto done
:codeanalyzer
                  REM  Build to test code analyzer
                  c:\orangec\temp\omake /DCOMPILER=MS fullbuild
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\tests\occpr
                  REM  generate code
                  omake
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  REM  analyze
                  omake > q
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\..\src
                  echo succeeded
                  goto done
:normal
                  REM  Primary build for Orange C
                  c:\orangec\temp\omake /DCOMPILER=%BUILD_PROFILE% /DORANGEC_ONLY=YES fullbuild -j:%PARALLEL%
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:%PARALLEL%
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DORANGEC_ONLY=YES /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:%PARALLEL%
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  mkdir c:\orangec\temp2
                  copy c:\orangec\bin\*.exe c:\orangec\temp2
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:%PARALLEL%
                  REM  in this last one we add in OCCIL so it will be in the install packages...
                  IF "%WITHDEBUG%" EQU "" (
                      c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% /DWITHMSDOS fullbuild -j:%PARALLEL%
                      goto cont
                  )
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:%PARALLEL%
:cont
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\temp\omake /DCOMPILER=OCC compare
                  IF %ERRORLEVEL% NEQ 0 (
                       goto error;
                  )
                  if "%TRAVIS_OS_NAME%" NEQ "" (
                       goto done;
                  )
                  cd ..\tests
                  omake -B -j:%PARALLEL% /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  omake /fzip7z.mak
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  echo succeeded
                  goto done
:error
     echo failed
     goto done
:done
     if "%TRAVIS_OS_NAME%" NEQ "" ( exit %ERRORLEVEL% )
