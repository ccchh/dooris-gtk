
all: dooris

dooris: dooris.c
	gcc -Wall -g `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json --libs libnotify` -DLIBNOTIFY -o dooris dooris.c

