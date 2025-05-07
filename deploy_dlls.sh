#!/bin/bash

echo "Preparing file structure"

dest="$2"

mkdir -p $dest
mkdir -p $dest/lib
mkdir -p $dest/share
mkdir -p $dest/share/icons
mkdir -p $dest/share/pango
mkdir -p $dest/share/glib-2.0
mkdir -p $dest/share/glib-2.0/schemas

echo
echo "Deploying DLLs"

opt="$1"

cp /$opt/bin/gdbus.exe $dest/
cp -R /$opt/lib/gdk-pixbuf-2.0 $dest/lib
rm $dest/lib/gdk-pixbuf-2.0/2.10.0/loaders/*.a
GDK_PIXBUF_MODULEDIR=$dest/lib/gdk-pixbuf-2.0/2.10.0/loaders
gdk-pixbuf-query-loaders --update-cache 
cp /$opt/bin/fc-* $dest/
cp -R /$opt/share/icons/Adwaita $dest/share/icons
cp -R /$opt/share/icons/hicolor $dest/share/icons
cp /$opt/share/glib-2.0/schemas/* $dest/share/glib-2.0/schemas/
glib-compile-schemas $dest/share/glib-2.0/schemas/
cp /$opt/bin/librsvg-2-2.dll $dest
cp /$opt/bin/libxml2-2.dll $dest
cp /$opt/bin/libiconv-2.dll $dest
cp /$opt/bin/libcharset-1.dll $dest
cp /$opt/bin/zlib1.dll $dest
cp /$opt/bin/liblzma-5.dll $dest

ldd $dest/JPaper.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" $dest/

echo "Done"