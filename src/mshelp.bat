set opf32=%ProgramFiles(x86)%

set ProgramFiles(x86) 2>Nul | Findstr/I "."
if errorlevel 1  set opf32 = %ProgramFiles%

PATH=c:\windows\microsoft.net\framework\v4.0.30319;%PATH%
cd msdnhelp\helpdownloader
msbuild /Property:configuration=Release helpdownloader.csproj
cd ..
cd helpviewerproject\hv2viewer
msbuild /Property:configuration=Release hv2viewer.csproj
msbuild /Property:configuration=Release_7 hv2viewer.csproj
cd ..\..\..
