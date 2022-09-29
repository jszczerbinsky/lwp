mkdir build
windres resource.rc -O coff -o resource.res
gcc -mwindows main.c parser.c debug.c window.c resource.res -lmingw32 -lSDL2main -lSDL2 -o lwp.exe