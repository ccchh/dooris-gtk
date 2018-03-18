
all: dooris

dooris: dooris.c
	$(CC) -Wall -g `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json-c --libs libnotify` -DLIBNOTIFY -o dooris dooris.c

clean:
	rm -f dooris
