@echo off

::use this cmd kill process,taskkill is unused
wmic process where name="Everthing_Win32.exe" call terminate

::waiting 5s
choice /t 5 /d y /n >nul
start "" "Everthing_Win32.exe"
exit