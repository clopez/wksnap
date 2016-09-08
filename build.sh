#!/bin/sh
set -x
gcc -o wksnap wksnap.c $(pkg-config --cflags --libs webkit2gtk-4.0)
