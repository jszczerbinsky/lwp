@echo off
title Uninstall Layered WallPaper

echo Are You sure, You want to remove Layered WallPaper from Your computer?
set /p val="(y/n) > "
if not %val%==y (
	exit
)

echo Killing all running instances...
taskkill /F /IM lwp.exe
echo.

echo Restarting explorer...
taskkill /im explorer.exe /f
start explorer.exe
echo.

echo Removing Layered WallPaper's registry keys..
reg delete "HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\layeredWallPaper" /f > nul
reg delete "HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run" /v "lwp" /f > nul
echo.

echo Removing Layered WallPaper's files...
rmdir /S /Q "%~dp0"
exit