#!/bin/bash

mkdir -p dlls
mkdir -p dlls/lib
mkdir -p dlls/share
mkdir -p dlls/share/icons
mkdir -p dlls/share/glib-2.0
mkdir -p dlls/share/glib-2.0/schemas

echo
PS3='Choose the correct version: '
options=("mingw64" "mingw32")
select opt in "${options[@]}"
do
    cp /$opt/bin/gdbus.exe ./dlls/
	cp -R /$opt/lib/gdk-pixbuf-2.0 ./dlls/lib
	rm ./dlls/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.a
	GDK_PIXBUF_MODULEDIR=./dlls/lib/gdk-pixbuf-2.0/2.10.0/loaders
	gdk-pixbuf-query-loaders --update-cache 
    cp /$opt/bin/fc-* ./dlls/
	cp -R /$opt/share/icons/Adwaita ./dlls/share/icons
	cp -R /$opt/share/icons/hicolor ./dlls/share/icons
	cp /$opt/share/glib-2.0/schemas/* ./dlls/share/glib-2.0/schemas/
	glib-compile-schemas ./dlls/share/glib-2.0/schemas/
	cp /$opt/bin/librsvg-2-2.dll ./dlls
	cp /$opt/bin/libxml2-2.dll ./dlls
	cp /$opt/bin/libiconv-2.dll ./dlls
	cp /$opt/bin/libcharset-1.dll ./dlls
	cp /$opt/bin/zlib1.dll ./dlls
	cp /$opt/bin/liblzma-5.dll ./dlls
	break;
done

ldd ./src/core/lwp.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" ./dlls/

echo "Done"