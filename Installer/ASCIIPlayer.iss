; -- Example1.iss --
; Demonstrates copying 3 files and creating an icon.

; SEE THE DOCUMENTATION FOR DETAILS ON CREATING .ISS SCRIPT FILES!

[Setup]
AppName=ASCIIPlayer
AppVersion=0.1.0
SetupIconFile=ASCIIPlayer.ico
DefaultDirName={pf}\ASCIIPlayer
DefaultGroupName=ASCIIPlayer
UninstallDisplayIcon={app}\ASCIIPlayer.exe
Compression=lzma2
SolidCompression=yes
OutputDir=output
OutputBaseFilename=ASCIIPlayerInstaller
ChangesAssociations=yes
LicenseFile=LICENSE.txt

[Files]
Source: "ascii_player.exe"; DestDir: "{app}"
Source: "fmodexL.dll"; DestDir: "{app}"
Source: "README.txt"; DestDir: "{app}"; Flags: isreadme

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
    GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: "{userdesktop}\ASCIIPlayer"; Filename: "{app}\ascii_player.exe"; Tasks: desktopicon
Name: "{group}\ASCIIPlayer"; Filename: "{app}\ascii_player.exe"

;[Registry]
;Root: HKCR; Subkey: ".mp3"; ValueType: string; ValueName: ""; ValueData: "ASCIIPlayer"; Flags: uninsdeletevalue
;Root: HKCR; Subkey: "ASCIIPlayer"; ValueType: string; ValueName: ""; ValueData: "ASCII Player"; Flags: uninsdeletekey
;Root: HKCR; Subkey: "ASCIIPlayer\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\ascii_player.exe,0"
;Root: HKCR; Subkey: "ASCIIPlayer\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\ascii_player.exe"" ""%1"""
