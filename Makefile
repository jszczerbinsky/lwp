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

install:
	mkdir -p $(INSTALL_PATH)
	cp -Rf build/* $(INSTALL_PATH)

ifeq ($(UNAME), DARWIN)

# It's unsafe to run this on Linux,
# because it will attempt to remove /, which is generally a bad idea.
# So for now, uninstall is only available on macOS.
uninstall:
	rm -rf $(INSTALL_PATH)

# Build the launchd plist file.
lwp.plist: lwp.template.plist
	cp lwp.template.plist lwp.plist
	sed -i "" "s/{{user}}/$$(whoami)/g" lwp.plist

# Launch lwp on login (macOS only).
install-launchd: lwp.plist
	mkdir -p ~/Library/LaunchAgents
	cp lwp.plist ~/Library/LaunchAgents
	launchctl load ~/Library/LaunchAgents/lwp.plist

# Stop lwp from running on login (macOS only).
uninstall-launchd:
	launchctl unload ~/Library/LaunchAgents/lwp.plist
	rm ~/Library/LaunchAgents/lwp.plist

endif

clean:
	rm -rf build
ifeq ($(UNAME), DARWIN)
	rm -rf lwp.plist
endif
