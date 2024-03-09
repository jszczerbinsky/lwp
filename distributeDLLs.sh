#!/bin/bash

mkdir -p dlls
mkdir -p dlls/lib
mkdir -p dlls/lib/gdk-pixbuf-2.0
mkdir -p dlls/lib/gdk-pixbuf-2.0/2.10.0

echo
PS3='Choose the correct version of gdbus: '
options=("/mingw64/bin/gdbus.exe" "/mingw32/bin/gdbus.exe")
select opt in "${options[@]}"
do
    echo "Preparing gdbus.exe..."
    cp $opt ./dlls/
    break;
done

echo "Preparing DLLs..."

ldd ./src/core/lwp.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" ./dlls/
cp /mingw64/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache ./dlls/lib/gdk-pixbuf-2.0/2.10.0/loaders.cache

echo "Done"