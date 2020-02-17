#!/bin/sh
rm *.o app
g++ -c main.cpp -I/usr/include/apr-1.0
gcc main.o -lapr-1 -lm -lpthread -lstdc++ -o app
