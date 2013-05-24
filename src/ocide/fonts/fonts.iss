; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=vera font
AppVerName=vera font
DefaultDirName=c:\tools\cc\ccide\font\vera font
DefaultGroupName=vera font
UninstallDisplayIcon={app}\MyProg.exe
Compression=lzma
SolidCompression=yes

[Files]
;uninsneveruninstall
Source: "C:\tools\cc\ccide\fonts\ttf-bitstream-vera-1.10\VeraMono.ttf"; DestDir: "{fonts}"; FontInstall: "Bitstream Vera Sans Mono"; Flags: onlyifdoesntexist uninsneveruninstall


