PREFIX = /usr/local

ifdef sysconfigdir
	SYSCONFIGDIR = $(sysconfigdir)
else
	SYSCONFIGDIR = /etc
endif

ifdef XDG_CONFIG_HOME
	HOMECONFIGDIR = $(XDG_CONFIG_HOME)
else
	HOMECONFIGDIR = ~/.config
endif

CC = gcc
CFLAGS = $(shell pkg-config --libs --cflags sdl2 x11)

default:
	mkdir -p build/$(PREFIX)/bin
	mkdir -p build/$(PREFIX)/share/lwp
	mkdir -p build/$(SYSCONFIGDIR)
	$(CC) main.c wallpaper.c window.c parser.c debug.c $(CFLAGS) -o build/$(PREFIX)/bin/lwp
	cp -R wallpapers build/$(PREFIX)/share/lwp
	cp default.cfg build/$(SYSCONFIGDIR)/lwp.cfg
	cp LICENSE build/$(PREFIX)/share/lwp

install:
	cp -R build/* /

uninstall:
	rm $(PREFIX)/bin/lwp
	rm -rf $(PREFIX)/share/lwp
	rm $(SYSCONFDIR)/lwp.cfg
