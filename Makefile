UNAME := $(shell uname | tr a-z A-Z)
FILES := $(shell find . -type f -name '*.c')
CFLAGS_SDL2 := $(shell pkg-config --libs --cflags sdl2)

# Configure build parameters based on the host OS.
ifeq ($(UNAME), DARWIN)
CC=clang
CFLAGS=$(CFLAGS_SDL2) -framework CoreGraphics -framework Foundation
INSTALL_PATH=/opt/lwp/
DEFAULT_CFG=defaultMac.cfg
else
CC=gcc
CFLAGS=$(CFLAGS_SDL2) -lX11
INSTALL_PATH=/
DEFAULT_CFG=default.cfg
endif

.DEFAULT_GOAL := build
.PHONY: build install run clean

build: $(FILES)
	mkdir -p build/usr/bin
	mkdir -p build/usr/share/lwp
	mkdir -p build/etc
	$(CC) -D__$(UNAME) $(FILES) -o build/usr/bin/lwp $(CFLAGS)
	cp -R wallpapers build/usr/share/lwp
	cp $(DEFAULT_CFG) build/etc/lwp.cfg
	cp LICENSE build/usr/share/lwp

install: build
	mkdir -p $(INSTALL_PATH)
	cp -Rf build/* $(INSTALL_PATH)

# It's unsafe to run this on Linux,
# because it will attempt to remove /, which is generally a bad idea.
# So for now, uninstall is only available on macOS.
ifeq ($(UNAME), Darwin)
uninstall:
	rm -rf $(INSTALL_PATH)
endif

clean:
	rm -rf build
