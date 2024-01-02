#!/bin/bash

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

mkdir -p dlls
ldd ./src/core/lwp.exe | grep '\/mingw.*\.dll' -o | xargs -I{} cp "{}" ./dlls/

echo "Done"