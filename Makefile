UNAME := $(shell uname | tr a-z A-Z)
FILES := $(shell find . -type f -name '*.c')

ifeq ($(UNAME), DARWIN)
CC=clang
CFLAGS=-D_THREAD_SAFE -I/opt/homebrew/include -I/opt/homebrew/include/SDL2 -L/opt/homebrew/lib -lSDL2 -framework CoreGraphics -framework Foundation
INSTALL_PATH=/opt/lwp/
DEFAULT_CFG=defaultMac.cfg
else
CC=gcc
CFLAGS=-lSDL2 -lX11
INSTALL_PATH=/
DEFAULT_CFG=default.cfg
endif

.DEFAULT_GOAL := build
.PHONY: build install run clean

build: $(FILES)
	mkdir -p build
	mkdir -p build/usr/bin
	mkdir -p build/usr/share/lwp
	mkdir -p build/etc
	$(CC) -D__$(UNAME) main.c wallpaper.c window.c parser.c debug.c -o build/usr/bin/lwp $(CFLAGS)
	cp -R wallpapers build/usr/share/lwp
	cp $(DEFAULT_CFG) build/etc/lwp.cfg
	cp LICENSE build/usr/share/lwp

install: build
	mkdir -p $(INSTALL_PATH)
	cp -Rf build/* $(INSTALL_PATH)

ifeq ($(UNAME), Darwin)
uninstall:
	rm -rf $(INSTALL_PATH)
endif

clean:
	rm -rf build