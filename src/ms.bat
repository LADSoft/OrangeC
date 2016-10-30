if NOT "%VCINSTALLDIR%"=="" goto noload

call "vcvars32.bat"

:noload

msbuild /Property:configuration=Release /Property:platform=win32
