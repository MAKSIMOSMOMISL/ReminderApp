[Setup]
AppName=Reminder App
AppVersion=1.0
DefaultDirName={pf}\ReminderApp
DefaultGroupName=Reminder App
UninstallDisplayIcon={app}\ReminderApp.exe
Compression=lzma2
SolidCompression=yes
OutputDir=installer
OutputBaseFilename=ReminderApp_Setup

[Files]
Source: "build\ReminderApp.exe"; DestDir: "{app}"
Source: "build\Qt6Core.dll"; DestDir: "{app}"
Source: "build\Qt6Gui.dll"; DestDir: "{app}"
Source: "build\Qt6Widgets.dll"; DestDir: "{app}"
Source: "build\Qt6Svg.dll"; DestDir: "{app}"
Source: "build\libwinpthread-1.dll"; DestDir: "{app}"
Source: "build\libgcc_s_seh-1.dll"; DestDir: "{app}"
Source: "build\libstdc++-6.dll"; DestDir: "{app}"
Source: "build\platforms\qwindows.dll"; DestDir: "{app}\platforms"

[Icons]
Name: "{group}\Reminder App"; Filename: "{app}\ReminderApp.exe"
Name: "{group}\Uninstall Reminder App"; Filename: "{uninstallexe}"
Name: "{commondesktop}\Reminder App"; Filename: "{app}\ReminderApp.exe"

[Run]
Filename: "{app}\ReminderApp.exe"; Description: "Запустить Reminder App"; Flags: postinstall nowait skipifsilent
