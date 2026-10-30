#define AppVersion "0.1.0"
[Setup]
AppId={{418E956D-1377-4CF3-989B-F2C442050822}
AppName=Voltage Garden
AppVersion={#AppVersion}
AppPublisher=Jerzy Audio
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=..\out\release
OutputBaseFilename=VoltageGarden-0.1.0-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
LicenseFile=..\LICENSES\AGPL-3.0.txt
UninstallDisplayName=Voltage Garden VST3
UninstallFilesDir={commonappdata}\Jerzy Audio\Voltage Garden\Uninstall
[Files]
Source: "..\out\package\Voltage Garden.vst3\*"; DestDir: "{app}\Voltage Garden.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\out\package\docs\*"; DestDir: "{commonappdata}\Jerzy Audio\Voltage Garden\docs"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\out\package\LICENSES\*"; DestDir: "{commonappdata}\Jerzy Audio\Voltage Garden\LICENSES"; Flags: ignoreversion recursesubdirs createallsubdirs
