@echo off
     set PARALLEL=%NUMBER_OF_PROCESSORS%
     if "%TRAVIS_OS_NAME%" NEQ "" (
        call "C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\Common7\Tools\VsDevCmd.bat"
     )
     if "%ORANGEC_HOME%" NEQ "" (
         del /Q ..\bin\*.* 2> NUL
         del /Q ..\lib\*.* 2> NUL
         del /Q ..\include\*.* 2> NUL
         del /Q ..\include\win32\*.* 2> NUL
         del /Q ..\include\sys\*.* 2> NUL
         omake -DCOMPILER=MS clean -j:%NUMBER_OF_PROCESSORS%
         omake -DCOMPILER=CLANG clean -j:%NUMBER_OF_PROCESSORS%
         omake -DCOMPILER=MINGW64 clean -j:%NUMBER_OF_PROCESSORS%
         set BUILD_PROFILE=MS
         set TESTS=TRUE
         set PARALLEL=%NUMBER_OF_PROCESSORS%
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
              IF "%BUILD_PROFILE%" EQU "LIBCXXTEST" goto libcxxtest
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
                  c:\orangec\temp\omake -j:%PARALLEL% /DCOMPILER=MS /DMSPDB=%MSPDB% fullbuild
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  echo succeeded
                  goto done
:libcxxtest
                  REM  Build to test libcxx
                  c:\orangec\temp\omake /DCOMPILER=MS fullbuild /DORANGEC_ONLY=YES -j:%PARALLEL%
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\temp\omake test
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
                  omake -j:1
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
                  mkdir c:\orangec\temp2 2> NUL
                  copy c:\orangec\bin\*.exe c:\orangec\temp2 2> NUL
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  REM  in this last one we add in OCCIL so it will be in the install packages...
                  IF "%WITHDEBUG%" NEQ "" (
                      omake /fzip.mak
                      goto nosecondbuild:
                  )
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:%PARALLEL%
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASSEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% /DWITHMSDOS fullbuild -j:%PARALLEL%
                  )
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\temp\omake /DCOMPILER=OCC compare
                  IF %ERRORLEVEL% NEQ 0 (
                       goto error;
                  )
:nosecondbuild
                  if "%TRAVIS_OS_NAME%" NEQ "" (
                       goto done;
                  )
                  IF "%TESTS%" EQU "" (
                       goto notests;
                  )
                  cd ..\tests
                  omake -B -j:%PARALLEL% /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
:notests
                  IF "%WITHDEBUG%" NEQ "" (
                      goto finish
                  )
                  omake /fzip7z.mak
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
:finish
                  echo succeeded
                  goto done
:error
     echo failed
     goto done
:done
     if "%TRAVIS_OS_NAME%" NEQ "" ( exit %ERRORLEVEL% )
