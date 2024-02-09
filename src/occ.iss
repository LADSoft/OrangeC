;
; inno setup installation script for WIN32 Orange C package
;
[Setup]
PrivilegesRequired=admin
PrivilegesRequiredOverridesAllowed=dialog
AppName=Orange C
AppVerName=Orange C Version {#GetEnv("OCC_VERSION")}
OutputBaseFileName=setup
AppPublisher=LADSoft
AppPublisherURL=https://ladsoft.tripod.com
AppSupportURL=https://ladsoft.tripod.com
AppUpdatesURL=https://ladsoft.tripod.com
DefaultDirName={autopf}\OrangeC
DefaultGroupName=OrangeC
SetupIconFile=c:\orangec\src\prj.ico
;AlwaysCreateUninstallIcon=yes
LicenseFile=License.txt
;BackColor=clRed
;BackColor2=clAqua
;WindowVisible= yes
AppCopyright=Copyright(C) LADSoft 1994-2019, All Rights Reserved.
WizardSmallImageFile=ladsoft1.bmp
WizardImageFile=ladsoftl.bmp
WizardImageBackColor=clAqua
DisableStartupPrompt=yes
InfoAfterFile=relnotes.txt
Compression=lzma
UninstallDisplayIcon={app}\prj.ico,1
uninstallable = not IsComponentSelected('main\portable')
ChangesAssociations=yes
ChangesEnvironment=yes
DisableWelcomePage=no

[Messages]
BeveledLabel=Orange C, Copyright (C) LADSoft, 1994-2024

[Types]
Name: "desktop"; Description: "Desktop Installation"
Name: "portable"; Description: "Portable Installation"

[Components]
Name: "main"; Description: "Main Files"; types: desktop portable; flags: fixed
Name: "main\examples"; Description: "Example Files"; types: desktop portable
Name: "main\desktop"; Description: "Desktop"; types: desktop; flags: exclusive
Name: "main\portable"; Description: "Portable"; types: portable; flags: exclusive

[Tasks]
Name: "addtopath"; Description: "Add Orange C to the PATH"; GroupDescription: "General:"; Flags: checkedonce;  Components: main\desktop

[Dirs]
;Name: "{%PUBLIC}\Orange C Projects"; Components: main\desktop
;Name: "{app}\appdata"; Components: main\portable

[Files]
Source: "C:\orangec\src\prj.ico\*.*"; DestDir: "{app}\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\license\*.*"; DestDir: "{app}\license\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\bin\*.*"; Excludes: "C:\orangec\bin\lsmsilcrtl.dll"; DestDir: "{app}\bin\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\bin\lsmsilcrtl.dll"; DestDir: "{app}\bin\"; Flags: IgnoreVersion gacinstall; Check: IsAdminInstallMode; StrongAssemblyName: "lsmsilcrtl, Version=1.0.0.0, Culture=neutral, PublicKeyToken=bc9b111235642d7d, ProcessorArchitecture=x86"
;Source: "C:\orangec\bin_7\*.*"; DestDir: "{app}\bin_7\"; Flags: IgnoreVersion;Components: main
;Source: "C:\orangec\bin_7\branding\*.*"; DestDir: "{app}\bin_7\branding"; Flags: IgnoreVersion;Components: main
;Source: "C:\orangec\bin_8\*.*"; DestDir: "{app}\bin_8\"; Flags: IgnoreVersion;Components: main
;Source: "C:\orangec\bin_8\branding\*.*"; DestDir: "{app}\bin_8\branding"; Flags: IgnoreVersion;Components: main
Source: "C:\orangec\help\*.*"; DestDir: "{app}\help\"; Flags: IgnoreVersion; Components: main
;Source: "C:\orangec\rule\*.rul"; DestDir: "{app}\rule\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\*.*"; DestDir: "{app}\include\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\c\sys\*.*"; DestDir: "{app}\include\c\sys"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\c\*.*"; DestDir: "{app}\include\c"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\c\win32\*.*"; DestDir: "{app}\include\c\win32\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\support\win32\*.*"; DestDir: "{app}\include\support\win32\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\include\support\orangec\*.*"; DestDir: "{app}\include\support\orangec\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\lib\*.*"; DestDir: "{app}\lib\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\lib\startup\*.*"; DestDir: "{app}\lib\startup\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\*.*"; DestDir: "{app}\doc\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\general\*.*"; DestDir: "{app}\doc\general\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\oasm\*.*"; DestDir: "{app}\doc\oasm\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\occ\*.*"; DestDir: "{app}\doc\occ\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\ogrep\*.*"; DestDir: "{app}\doc\ogrep\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\olink\*.*"; DestDir: "{app}\doc\olink\"; Flags: IgnoreVersion; Components: main
Source: "C:\orangec\doc\omake\*.*"; DestDir: "{app}\doc\omake\"; Flags: IgnoreVersion; Components: main

Source: "C:\orangec\bin\lscrtl.dll"; DestDir: "{sys}"; Components: main\desktop; Check: IsAdminInstallMode;

Source: "C:\orangec\examples\*.*"; DestDir: "{app}\examples\"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\msdos\*.*"; DestDir: "{app}\examples\msdos\"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\*.*"; DestDir: "{app}\examples\windows examples\"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\atc\*.*"; DestDir: "{app}\examples\windows examples\atc"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\listview\*.*"; DestDir: "{app}\examples\windows examples\listview"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\xmlview\*.*"; DestDir: "{app}\examples\windows examples\xmlview"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\RCDemo\*.*"; DestDir: "{app}\examples\windows examples\RCDemo"; Flags: IgnoreVersion; Components: main\examples
Source: "C:\orangec\examples\win32\huff\*.*"; DestDir: "{app}\examples\windows examples\huff"; Flags: IgnoreVersion; Components: main\examples

[Icons]
Name: "{group}\C Language Help"; Filename: "{app}\help\chelp.chm"; Components: main\desktop;
Name: "{group}\Runtime Help"; Filename: "{app}\help\crtl.chm"; Components: main\desktop;
Name: "{group}\Tools Help"; Filename: "{app}\help\tools.chm"; Components: main\desktop;

[Run]

[Registry]
;Root: HKLM; Subkey: "Software\LADSoft"; Flags: uninsdeletekeyifempty; Components: main\desktop;
;Root: HKLM; SubKey: "Software\LADSoft\ORANGEC"; Valuetype: string; ValueName: "InstallPath"; ValueData: "{app}"; Components: main\desktop;

Root: HKCU; SubKey: "Environment"; ValueType: string; ValueName: "ORANGEC"; ValueData: "{app}"; Flags: uninsdeletevalue; Components: main\desktop; Check: not IsAdminInstallMode
Root: HKLM; SubKey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: string; ValueName: "ORANGEC"; ValueData: "{app}"; Flags: uninsdeletevalue; Components: main\desktop; Check: IsAdminInstallMode


[UninstallDelete]

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

// sets up the environment so we can spawn the IDE from the setup program without problems
procedure SetEnvPath;
var
  WorkingVar : String;
begin
  RegQueryStringValue(HKEY_CURRENT_USER, 'Environment', 'Path', WorkingVar);
  Insert(ExpandConstant('{app}\bin;'), WorkingVar, 1);
  if not SetEnvironmentVariable('PATH', WorkingVar) then
    MsgBox(SysErrorMessage(DLLGetLastError), mbError, MB_OK);
  if not SetEnvironmentVariable('ORANGEC', ExpandConstant('{app}')) then
    MsgBox(SysErrorMessage(DLLGetLastError), mbError, MB_OK);
end;

function InitializeSetup(): Boolean;                                begin
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
  RootKey: integer;
  Key: string;
begin
  RootKey := HKEY_CURRENT_USER;
  Key := 'Environment';
  if IsAdminInstallMode then
    Begin
      RootKey := HKEY_LOCAL_MACHINE;
      Key := 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';
    End;
  RegQueryStringValue(RootKey, Key, 'Path', WorkingPath);
  if Pos(Path, workingPath) = 0 then
    begin
      Insert(Path, WorkingPath, 1);
      RegWriteExpandStringValue(RootKey, Key, 'Path', WorkingPath);
    end;
end;
procedure RemoveFromPath(Path : String);
var
  WorkingPath : String;
  RootKey: integer;
  Key: string;
begin
  RootKey := HKEY_CURRENT_USER;
  Key := 'Environment';
  if IsAdminInstallMode then
    Begin
      RootKey := HKEY_LOCAL_MACHINE;
      Key := 'SYSTEM\CurrentControlSet\Control\Session Manager\Environment';
    End;
  RegQueryStringValue(RootKey, Key, 'Path', WorkingPath);
  if Pos(Path, workingPath) <> 0 then
    begin
      Delete(WorkingPath, Pos(Path, workingPath), Length(Path));
      RegWriteExpandStringValue(RootKey, Key, 'Path', WorkingPath);
    end;
end;
procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then
    begin
      DeleteFile(ExpandConstant('{app}\help\tools.chm:Zone.Identifier'));
      DeleteFile(ExpandConstant('{app}\help\crtl.chm:Zone.Identifier'));
      DeleteFile(ExpandConstant('{app}\help\chelp.chm:Zone.Identifier'));
      if Not IsComponentSelected('main\portable') Then
        Begin
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
  if IsComponentSelected('main\portable') Then
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
    End;
  if not IsAdminInstallMode then
    Begin
      retval := retval + NewLine + NewLine + 'OCCIL will not be completely installed.' + NewLine +  'Install for all users for complete support.';
    End;
  result := retval;
end;


procedure CurPageChanged(CurPageID: Integer);
begin
end;

