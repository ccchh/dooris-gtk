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

Use the following command for getting the sources:

    git clone https://github.com/ccchh/dooris-gtk.git

Change into the new directory:

    cd dooris-gtk

Then compile the sources (take a look below before! A Make based compilation is possible too!):

    gcc `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json` -o dooris dooris.c

To compile with libnotify support use the following command:

    gcc `pkg-config --cflags --libs gtk+-2.0 --libs libcurl --libs json --libs libnotify` -DLIBNOTIFY -o dooris dooris.c

There is a Makefile included so you maybe just want to run:

    make

The Makefile uses libnotify. If you don't need/want libnotify support compile dooris like described above.

Configuration:
--------------

Configuration needs to be done before compiling. There are some parameters you can set in the head of this file where variables are declared. A configuration file will at this point make no sense.

Usage:
------

Run dooris with:

    ./dooris & 

Status Icon "Pesthörnchen" colors:

- Black  = Hackers inside; Door closed
- Yellow = Hackers inside; Door open
- Red    = No Hackers inside; Door closed
- Orange = No Hackers inside; Door open!!!
