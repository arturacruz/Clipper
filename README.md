# Editor

A simple text/code editor using GTK.  
Mainly made for fun and learning C.  

Compiling:  
```sh
gcc -Wall -pedantic $(pkg-config --cflags gtk4) -Og -o main main.c $(pkg-config --libs gtk4)
```