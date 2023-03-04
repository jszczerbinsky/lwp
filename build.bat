@echo off
if exist data rmdir /S /Q data
mkdir data || goto err
windres resource.rc -O coff -o resource.res || goto err
gcc -mwindows main.c wallpaper.c parser.c debug.c window.c resource.res -lmingw32 -lshcore -lSDL2main -lSDL2 -o data\lwp.exe || goto err
xcopy wallpapers data\wallpapers\ /E /Y || goto err
copy uninstall.bat data || goto err
copy defaultWin.cfg data || goto err
copy LICENSE data\LICENSE.txt || goto err
echo.
echo ---                           Done!                                  ---
copy SDL2.dll data || echo --- Remember to put SDL2.dll into 'data' directory before installing ---
pause>nul
exit

:err
echo ---                           Failed :(                              ---
pause>nul
exit /b 1