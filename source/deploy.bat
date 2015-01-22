del /F /S /Q .\..\..\K70Tray_latest_build\*.*
del /S /Q .\..\..\K70Tray_latest_build.zip
rmdir /S /Q .\..\..\K70Tray_latest_build

mkdir .\..\..\K70Tray_latest_build

mkdir .\..\..\K70Tray_latest_build\Themes
xcopy /E .\Themes .\..\..\K70Tray_latest_build\Themes

mkdir .\..\..\K70Tray_latest_build\Layouts
xcopy /E .\Layouts .\..\..\K70Tray_latest_build\Layouts

xcopy .\config.xml .\..\..\K70Tray_latest_build\
xcopy .\..\Release\K70Tray.exe .\..\..\K70Tray_latest_build\

"C:\Program Files\WinRAR\WinRAR.exe" a -r -u -x*\log.txt .\..\..\K70Tray_latest_build.rar .\..\..\K70Tray_latest_build\*.*


