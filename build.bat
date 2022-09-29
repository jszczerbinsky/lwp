@echo off
mkdir data
windres resource.rc -O coff -o resource.res
gcc -mwindows main.c parser.c debug.c window.c resource.res -lmingw32 -lSDL2main -lSDL2 -o data\lwp.exe
xcopy wallpapers data\wallpapers \E
copy defaultWin.cfg data
copy LICENSE data\LICENSE.txt
echo.
echo --- Remember to put SDL2.dll into 'data' directory before installing ---
pause>nul