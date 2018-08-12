[Setup]
AppName=LED Cube Simulator
AppVersion=1.0
DefaultDirName={pf}\LED_Cube
DefaultGroupName=LED_Cube
UninstallDisplayIcon={app}\LED_Cube_Simulator.exe
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64

[Files]
Source: "dll\*"; DestDir: "{app}"
Source: "led_cube_simulator.exe"; DestDir: "{app}"

[Icons]
Name: "{group}\My Program"; Filename: "{app}\LED_Cube_Simulator.exe"

