@echo on
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
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  path="%ORIGPATH%"
                  cd ..\tests
                  omake -B /Coccil /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  goto success
              )
              IF "%BUILD_PROFILE%" EQU "MSDEBUGBUILD" (
                  REM  Build with Microsoft PDB files
                  c:\orangec\temp\omake /DCOMPILER=MS /DMSPDB=%MSPDB% fullbuild
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  goto success
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
                  goto success
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
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DORANGEC_ONLY=YES /DVIAASSEMBLY=%VIAASEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:4
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DORANGEC_ONLY=YES /DVIAASSEMBLY=%VIAASEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
                  IF %ERRORLEVEL% NEQ 0 (
                     goto error;
                  )
                  c:\orangec\bin\occ /V
                  copy omake\omake.exe \orangec\temp
                  c:\orangec\temp\omake /DCOMPILER=OCC clean -j:4
                  REM  in this last one we add in OCCIL so it will be in the install packages...
                  IF "%WITHDEBUG%" EQU "" (
                      c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% /DWITHMSDOS fullbuild -j:4
                      goto cont
                  )
                  c:\orangec\temp\omake /DNOMAKEDIR /DCOMPILER=OCC /DVIAASSEMBLY=%VIAASEMBLY% /DLSCRTL=%LSCRTL% /DWITHDEBUG=%WITHDEBUG% fullbuild -j:4
:cont
                  IF %ERRORLEVEL% NEQ 0 (
                       goto error;
                  )
                  path="%ORIGPATH%"
                  cd ..\tests
                  omake -B -j:4 /DCOMPILER=OCC
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  cd ..\src
                  omake /fzip7z.mak
                  IF %ERRORLEVEL% NEQ 0 (
                      goto error;
                  )
                  goto success
             )
:error
     echo failed
     goto done
:success
     echo succeeded
:done