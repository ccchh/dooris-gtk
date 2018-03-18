dooris-gtk
==========

Menubar item for GTK2 showing the Dooris status compatible with the SpaceAPI version 0.13.

Installation:
-------------

Requirements:

- Standard tools like gcc, pkg-config etc.
- curl
- gtk+-2.0
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

Configuration needs to be done before compiling. There are some parameters you can set in the head of the dooris.c file where variables are declared. A configuration file will at this point make no sense.

Usage:
------

Run dooris with:

    ./dooris & 

Status Icon "Pesthörnchen" colors:

- Red = Door closed
- Yellow = Door open

