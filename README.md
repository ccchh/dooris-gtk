dooris-gtk
==========

Menubar item for GTK showing the Dooris status

Installation:
-------------

Requirements:

- Standard tools like gcc, pkg-config etc.
- curl
- gtk+
- json-c
- libnotify - Only needed when compiling with libnotify support.

Download the file to some directory. cd to it and run the following command:

$ gcc `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json` -o dooris dooris.c

To compile with libnotify support use the following command:

$ gcc `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json --libs libnotify` -DLIBNOTIFY -o dooris dooris.c

There is a Makefile included so you maybe just want to run:

$ make

The Makefile uses libnotify. If you don't need/want libnotify support compile dooris like described above.

Configuration:
--------------

Configuration needs to be done before compiling. There are some parameters you can set in the head of this file where variables are declared. A configuration file will at this point make no sense.

Usage:
------

Run dooris with:

$ ./dooris & 

Status Icon "Pesthörnchen" colors:

- Black  = Hackers inside; Door closed
- Yellow = Hackers inside; Door open
- Red    = No Hackers inside; Door closed
- Orange = No Hackers inside; Door open!!!
