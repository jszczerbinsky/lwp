default:
	gcc main.c window.c parser.c debug.c -lSDL2 -lX11
install:
	cp a.out /usr/bin/lwp
	mkdir -p /usr/share/lwp
	cp -R wallpapers /usr/share/lwp
	cp default.cfg /etc/lwp.cfg
