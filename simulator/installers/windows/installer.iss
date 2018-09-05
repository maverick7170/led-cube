[Setup]
AppName=LED Cube Simulator
AppVersion=1.0
DefaultDirName={pf}\LED_Cube
DefaultGroupName=LED_Cube
UninstallDisplayIcon={app}\LED_Cube_Simulator.exe
Compression=lzma2
SolidCompression=yes
ArchitecturesInstallIn64BitMode=x64
DisableWelcomePage=no

[Messages]
WelcomeLabel2 = This will install [name/ver] on your computer (restart after installation)! Please check out https://github.com/maverick7170/led-cube for more information!

[Files]
Source: "dll\*"; DestDir: "{app}"
Source: "..\..\led_cube_simulator.exe"; DestDir: "{app}"
Source: "..\..\resources\*"; DestDir: "{app}\resources"
Source: "..\..\resources\mario\*"; DestDir: "{app}\resources\mario"
Source: "..\..\resources\shaders\*"; DestDir: "{app}\resources\shaders"
Source: "..\..\examples\c++\*.exe"; DestDir: "{app}\examples\c++"
Source: "..\..\examples\matlab\*.m"; DestDir: "{app}\examples\matlab"
Source: "..\..\examples\matlab\*.h"; DestDir: "{app}\examples\matlab"
Source: "..\..\examples\matlab\*.dll"; DestDir: "{app}\examples\matlab"
Source: "dll\sfml-network-2.dll"; DestDir: "{app}\examples\matlab"
Source: "..\..\examples\python\*.py"; DestDir: "{app}\examples\python"

[Icons]
Name: "{group}\My Program"; Filename: "{app}\LED_Cube_Simulator.exe"

[Registry]
Root: HKLM; Subkey: "SYSTEM\CurrentControlSet\Control\Session Manager\Environment"; ValueType: expandsz; ValueName: "Path"; ValueData: "{olddata};{app}"
