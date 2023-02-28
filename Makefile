UNAME := $(shell uname | tr a-z A-Z)
FILES := $(shell find . -type f -name '*.c')

ifeq ($(UNAME), Darwin)
	UNAME := DARWIN
endif

ifdef sysconfdir
	SYSCONFIGDIR = $(sysconfdir)
else ifeq ($(UNAME), LINUX)
	SYSCONFIGDIR = /etc
endif

ifdef XDG_CONFIG_HOME
	HOMECONFIGDIR = $(XDG_CONFIG_HOME)
else
	HOMECONFIGDIR = ~/.config
endif

# Configure build parameters based on the host OS.
ifeq ($(UNAME), DARWIN)
CC = clang
CFLAGS = $(shell pkg-config --libs --cflags sdl2) -framework CoreGraphics -framework Foundation
PREFIX = /opt/lwp
SYSCONFIGDIR = $(PREFIX)
DEFAULT_CFG = defaultMac.cfg
else
CC = gcc
CFLAGS = $(shell pkg-config --libs --cflags sdl2 x11)
PREFIX = /usr/local
DEFAULT_CFG = default.cfg
endif

.DEFAULT_GOAL := build
.PHONY: build install run clean

build: $(FILES)
	mkdir -p build/$(PREFIX)/bin
	mkdir -p build/$(PREFIX)/share/lwp
	mkdir -p build/$(SYSCONFIGDIR)

	$(CC) $(FILES) -D__$(UNAME) $(CFLAGS) -o build/$(PREFIX)/bin/lwp
	cp -R wallpapers LICENSE build/$(PREFIX)/share/lwp/
	cp $(DEFAULT_CFG) build/$(SYSCONFIGDIR)/lwp.cfg
	cp LICENSE build/$(PREFIX)/share/lwp/

install:
	cp -R build/* /

ifeq ($(UNAME), LINUX)

uninstall:
	rm $(PREFIX)/bin/lwp
	rm -r $(PREFIX)/share/lwp
	rm $(SYSCONFIGDIR)/lwp.cfg

endif

ifeq ($(UNAME), DARWIN)

uninstall:
	rm -r $(PREFIX)

# Build the launchd plist file.
lwp.plist: lwp.template.plist
	cp lwp.template.plist lwp.plist
	sed -i "" "s/{{user}}/$$(whoami)/g" lwp.plist
	sed -i "" "s^{{binpath}}^$(PREFIX)/bin/lwp^g" lwp.plist

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
