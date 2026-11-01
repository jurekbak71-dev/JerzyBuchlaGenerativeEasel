#define AppVersion "0.1.0"
[Setup]
AppId={{418E956D-1377-4CF3-989B-F2C442050822}
AppName=Jerzy Voltage Garden
AppVersion={#AppVersion}
AppPublisher=Jerzy Audio
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
PrivilegesRequired=admin
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
OutputDir=..\out\release
OutputBaseFilename=JerzyVoltageGarden-0.1.0-Windows-x64-Setup
Compression=lzma2
SolidCompression=yes
LicenseFile=..\LICENSES\AGPL-3.0.txt
UninstallDisplayName=Jerzy Voltage Garden VST3
UninstallFilesDir={commonappdata}\Jerzy Audio\Jerzy Voltage Garden\Uninstall
[Files]
Source: "..\out\package\Jerzy Voltage Garden.vst3\*"; DestDir: "{app}\Jerzy Voltage Garden.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\out\package\docs\*"; DestDir: "{commonappdata}\Jerzy Audio\Jerzy Voltage Garden\docs"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\out\package\LICENSES\*"; DestDir: "{commonappdata}\Jerzy Audio\Jerzy Voltage Garden\LICENSES"; Flags: ignoreversion recursesubdirs createallsubdirs
