copy omake.exe \orangec\temp
\orangec\temp\omake -j:4 -DCOMPILER=CLANG clean
\orangec\temp\omake -j:4 -DCOMPILER=CLANG fullbuild
if %ERRORLEVEL% neq 0 goto :exit
copy omake\omake.exe \orangec\temp
copy \orangec\bin\lscrtl.dll \orangec\temp
\orangec\temp\omake -DCOMPILER=OCC clean -j:4
\orangec\temp\omake -DCOMPILER=OCC -DNOMAKEDIR fullbuild -j:4
if %ERRORLEVEL% neq 0 goto :exit
copy omake\omake.exe \orangec\temp
copy \orangec\bin\lscrtl.dll \orangec\temp
\orangec\temp\omake -DCOMPILER=OCC clean -j:4
\orangec\temp\omake -DCOMPILER=OCC -DNOMAKEDIR fullbuild -j:4
if %ERRORLEVEL% neq 0 goto :exit
copy omake\omake.exe \orangec\temp
copy \orangec\bin\lscrtl.dll \orangec\temp
\orangec\temp\omake -DCOMPILER=OCC clean -j:4
\orangec\temp\omake -DCOMPILER=OCC -DNOMAKEDIR fullbuild -j:4
if %ERRORLEVEL% neq 0 goto :exit
echo succeeded
:exit