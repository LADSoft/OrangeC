;
; inno setup installation script for WIN32 Orange C package
;
[Setup]
PrivilegesRequired=admin
AppName=Orange C MSIL Version
AppVerName=Orange C MSIL Version 6.21
OutputBaseFileName=setup
AppPublisher=LADSoft
AppPublisherURL=https://ladsoft.tripod.com
AppSupportURL=https://ladsoft.tripod.com
AppUpdatesURL=https://ladsoft.tripod.com
DefaultDirName={pf}\Orange C MSIL
DefaultGroupName=Orange C MSIL
SetupIconFile=c:\orangec\src\prj.ico
;AlwaysCreateUninstallIcon=yes
LicenseFile=License.txt
;BackColor=clRed
;BackColor2=clAqua
;WindowVisible= yes
AppCopyright=Copyright(C) LADSoft 2016-2017, All Rights Reserved.
WizardSmallImageFile=ladsoft1.bmp
WizardImageFile=ladsoftl.bmp
WizardImageBackColor=clAqua
DisableStartupPrompt=yes
Compression=lzma
UninstallDisplayIcon={app}\bin\occil.exe,1
ChangesAssociations=yes
ChangesEnvironment=yes

; uncomment the following line if you want your installation to run on NT 3.51 too.
; MinVersion=4,3.51

[Messages]
BeveledLabel=Orange C MSIL, Copyright (C) LADSoft, 2016-2017

[Tasks]
Name: "addtopath"; Description: "Add Orange C MSIL to the path"; GroupDescription: "General:"; Flags: checkedonce;

[Files]
Source: "C:\occil\license.txt"; DestDir: "{app}\"; Flags: IgnoreVersion;
Source: "C:\occil\berkely.lic"; DestDir: "{app}\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib.txt"; DestDir: "{app}\"; Flags: IgnoreVersion;
Source: "C:\occil\occil.txt"; DestDir: "{app}\"; Flags: IgnoreVersion;
Source: "C:\occil\copying"; DestDir: "{app}\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\occil.cfg"; DestDir: "{app}\bin\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\occil.exe"; DestDir: "{app}\bin\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\netlink.exe"; DestDir: "{app}\bin\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\occmsil.dll"; DestDir: "{app}\bin\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\lscrtlil.dll"; DestDir: "{app}\bin\"; Flags: IgnoreVersion;
Source: "C:\occil\bin\lsmsilcrtl.dll"; DestDir: "{app}\bin\"; Flags: IgnoreVersion gacinstall; StrongAssemblyName: "lsmsilcrtl, Version=1.0.0.0, Culture=neutral, PublicKeyToken=bc9b111235642d7d, ProcessorArchitecture=x86"
Source: "C:\occil\include\*.*"; DestDir: "{app}\include\"; Flags: IgnoreVersion;
Source: "C:\occil\include\sys\*.*"; DestDir: "{app}\include\sys"; Flags: IgnoreVersion;
Source: "C:\occil\include\win32\*.*"; DestDir: "{app}\include\win32\"; Flags: IgnoreVersion;

Source: "C:\occil\DotNetPELib\*.*"; DestDir: "{app}\DotNetPELib\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib\test\*.*"; DestDir: "{app}\DotNetPELib\test\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib\ObjTest\*.*"; DestDir: "{app}\DotNetPELib\ObjTest\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib\netlink\*.*"; DestDir: "{app}\DotNetPELib\netlink\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib\util\*.*"; DestDir: "{app}\DotNetPELib\util\"; Flags: IgnoreVersion;
Source: "C:\occil\DotNetPELib\docs\*.*"; DestDir: "{app}\DotNetPELib\docs\"; Flags: IgnoreVersion;

Source: "C:\occil\examples\*.c"; DestDir: "{app}\examples\"; Flags: IgnoreVersion;
Source: "C:\occil\examples\cc386\*.*"; DestDir: "{app}\examples\cc386\"; Flags: IgnoreVersion;
Source: "C:\occil\examples\sqlite3\*.*"; DestDir: "{app}\examples\sqlite3\"; Flags: IgnoreVersion;
Source: "C:\occil\examples\msil\*.*"; DestDir: "{app}\examples\msil\"; Flags: IgnoreVersion;

[Registry]

Root: HKCU; SubKey: "Environment"; ValueType: string; ValueName: "OCCIL_ROOT"; ValueData: "{app}";

[Code]
#ifdef UNICODE
  #define AW "W"
#else
  #define AW "A"
#endif
function SetEnvironmentVariable(lpName: string; lpValue: string): BOOL;
  external 'SetEnvironmentVariable{#AW}@kernel32.dll stdcall';

function SHChangeNotify(cmd:Integer; flags:Integer; dwItem1:Integer; dwItem2:Integer) : Integer;
  external 'SHChangeNotify@shell32.dll stdcall';

procedure SetEnvPath;
begin
  if not SetEnvironmentVariable('PATH', ExpandConstant('{app}')) then
    MsgBox(SysErrorMessage(DLLGetLastError), mbError, MB_OK);
end;
function InitializeSetup(): Boolean;
begin
  Result := True;
end;

procedure DeinitializeSetup();
var
  FileName: String;
  ResultCode: Integer;
begin
end;
function InitializeUninstall(): Boolean;
begin
  Result := True;
end;

procedure DeinitializeUninstall();
begin
end;

procedure AddToPath(Path : String);
var
  WorkingPath : String;
begin
  RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', WorkingPath);
  if Pos(Path, workingPath) = 0 then
    begin
      Insert(Path, WorkingPath, 1);
      RegWriteExpandStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', WorkingPath);
    end;
end;
procedure RemoveFromPath(Path : String);
var
  WorkingPath : String;
begin
  RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', WorkingPath);
  if Pos(Path, workingPath) <> 0 then
    begin
      Delete(WorkingPath, Pos(Path, workingPath), Length(Path));
      RegWriteExpandStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', WorkingPath);
    end;
end;
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
    begin
          if IsTaskSelected('addtopath') Then
            Begin
              AddToPath(ExpandConstant('{app}\bin;'));
            end;
          if IsTaskSelected('fileassociation') Then
            Begin
              SHChangeNotify($08000000, 0, 0,0);
            end;
   end;
end;
procedure CurUninstallStepChanged(CurUninstallStep: TUninstallStep);
begin
  if CurUninstallStep = usUninstall then
    begin
      Begin
        RemoveFromPath(ExpandConstant('{app}\bin;'));
      End
    end;
end;

function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Result := True;
end;

function ShouldSkipPage(PageID: Integer): Boolean;
begin
  if IsComponentSelected('main\memstick') Then
    Begin
       if PageID = wpSelectProgramGroup then
         Begin
           result := true;
         End
       Else
         Begin
          result := false;
         End
    end
  else
    Begin
      result := false;
    End;
end;
function UpdateReadyMemo(Space, NewLine, MemoUserInfoInfo, MemoDirInfo, MemoTypeInfo, MemoComponentsInfo, MemoGroupInfo, MemoTasksInfo: String): String;
var
  retval : string;
begin
  retval := MemoDirInfo + NewLine + NewLine + MemoTypeInfo ;
  if IsComponentSelected('main\desktop') Then
    Begin
      retval := retval + NewLine + NewLine + MemoTasksInfo + NewLine + NewLine + MemoGroupInfo;
    End
    result := retval;
end;


procedure CurPageChanged(CurPageID: Integer);
begin
end;

