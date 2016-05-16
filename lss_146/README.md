A C implementation of the UNIX command "ls -lS"

Features options:

-a show hidden files
-A hide hidden files
-L dereference links

Compile:

gcc -std=gnu99 -o lss lss.c

Usage:

lss [options...] [files...]
