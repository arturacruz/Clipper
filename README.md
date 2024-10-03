# Clipper

A simple text/code editor using GTK, mainly made for fun and learning C.  

You must have GCC and GTK4 installed.

Compiling:

```sh
gcc -Wall -pedantic $(pkg-config --cflags gtk4) -Og -o main main.c $(pkg-config --libs gtk4)
```