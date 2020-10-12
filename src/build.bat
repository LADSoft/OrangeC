@echo off
     if (%TRAVIS_OS_NAME% NEQ "") (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
     )
              cd c:\orangec\src
              echo WScript.Echo(Math.floor(new Date().getTime()/1000)); > %temp%\time.js
              for /f %%i in ('cscript //nologo %temp%\time.js') do set SOURCE_DATE_EPOCH=%%i
              del %temp%\time.js
              copy omake.exe \orangec\temp
              call c:\orangec\appveyorversion.bat
              IF "%BUILD_PROFILE%" EQU "OCCIL" (
                  REM  alternate build with OCCIL
                  c:\orangec\temp\omake /DCOMPILER=CLANG fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:4
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
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
                  exit
              )
              IF "%BUILD_PROFILE%" EQU "MSDEBUGBUILD" (
                  REM  Build with Microsoft PDB files
                  c:\orangec\temp\omake /DCOMPILER=MS /DMSPDB=%MSPDB% fullbuild
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  echo succeeded
                  exit
              )
              IF "%BUILD_PROFILE%" EQU "CODEANALYZER" (
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
                  exit
              )
              IF "%BUILD_PROFILE%" NEQ "TEST" (
                  REM  Primary build for Orange C
                  c:\orangec\temp\omake /DCOMPILER=%BUILD_PROFILE% /DORANGEC_ONLY=YES fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:4
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DORANGEC_ONLY=YES /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  mkdir c:\orangec\temp2
                  copy c:\orangec\bin\*.exe c:\orangec\temp2
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:4
                  REM  in this last one we add in OCCIL so it will be in the install packages...
                  IF "%WITHDEBUG%" EQU "" (
                      c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% /DWITHMSDOS fullbuild -j:4
                      goto cont
                  )
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
:cont
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\temp\omake /DCOMPILER=OCC compare
                  IF %ERRORLEVEL% NEQ 0 (
                       goto error;
                  )
                  cd ..\tests
                  rem -j4 here seems risky
                  omake -B /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  omake /fzip7z.mak
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  echo succeeded
                  exit
             )
:error
     echo failed
     goto done
:done
     if (%TRAVIS_OS_NAME% NEQ "") ( exit %ERRORLEVEL% )
