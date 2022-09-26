default:
	gcc main.c -lSDL2 -lX11 -lconfig
install:
	cp a.out /usr/bin/lwp
	mkdir -p /usr/share/lwp
	cp -R wallpapers /usr/share/lwp
	cp default.cfg /etc/lwp.cfg
