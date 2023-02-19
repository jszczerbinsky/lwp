default:
	mkdir -p build
	mkdir -p build/usr
	mkdir -p build/usr/bin
	mkdir -p build/usr/share
	mkdir -p build/usr/share/lwp
	mkdir -p build/etc
	gcc main.c window.c parser.c debug.c -lSDL2 -lX11 -o build/usr/bin/lwp
	cp -R wallpapers build/usr/share/lwp
	cp default.cfg build/etc
	cp LICENSE build/usr/share/lwp
install:
	cp -R build/* /
