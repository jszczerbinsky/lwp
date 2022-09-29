@echo off
cd %SystemRoot%\system32
title Install LWP

call :logo
net session >nul 2>&1
if not %ERRORLEVEL% equ 0 (
    echo Run me as admin
	pause>nul
	exit
)

call :logo
echo.
echo If You enjoy lwp, You can give it a start on github
echo https://github.com/jszczerbinsky/lwp
echo.
echo Lwp will be installed on Your computer
echo Click any key to display the license
pause > nul

cls
echo ________________________________________________________________________________________________________________________
type "%~dp0\data\LICENSE.txt"
echo ________________________________________________________________________________________________________________________
echo.
echo By continuing the installation You accept the license conditions
echo.
set /p val="Continue? (y/n) > "
if not %val%==y (
	call :logo
	echo You have to accept the license to continue installation
	echo Installator will exit now
	echo.
	echo Press any key to continue...
	pause>nul
	exit
)
call :logo
echo Starting installation
echo.
path=%ProgramFiles%
echo Copying files...
robocopy "%~dp0\data" "%path%\lwp" /E  /NFL /NDL /NJH /NJS /nc /ns /np
echo Adding lwp to autorun...
reg add "HKEY_LOCAL_MACHINE\Software\Microsoft\Windows\CurrentVersion\Run" /v "lwp" /t REG_EXPAND_SZ /d "%path%\lwp\lwp.exe" /f > nul
echo.
echo Lwp has been successfuly installed on Your computer!
start "" "%path%\lwp\lwp.exe"
pause > nul
exit

:logo
cls
echo.
echo.
echo "                                                    __ _       ______                                                 "
echo "                                                   / /| |     / / __ \                                                "
echo "                                                  / / | | /| / / /_/ /                                                "
echo "                                                 / /__| |/ |/ / ____/                                                 "
echo "                                                /_____/__/|__/_/                                                      "
echo.
echo.                   
exit /B 0