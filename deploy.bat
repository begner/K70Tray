del /F /S /Q .\K70Tray_latest_build\*.*
del /S /Q .\K70Tray_latest_build.zip
rmdir /S /Q K70Tray_latest_build

mkdir .\K70Tray_latest_build

mkdir .\K70Tray_latest_build\Themes
xcopy /E source\K70Tray\Themes .\K70Tray_latest_build\Themes

mkdir .\K70Tray_latest_build\Layouts
xcopy /E source\K70Tray\Layouts .\K70Tray_latest_build\Layouts

xcopy source\K70Tray\config.xml .\K70Tray_latest_build\
xcopy source\Release\K70Tray.exe .\K70Tray_latest_build\

"C:\Program Files\WinRAR\WinRAR.exe" a -r -u -x*\log.txt .\K70Tray_latest_build.rar .\K70Tray_latest_build\*.*


