#!/bin/sh
rm *.o app
g++ -c main.cpp -I../apr-1.6.3/include/
gcc main.o -L../apr-1.6.3/inst/lib/ -lapr-1 -lm -lpthread -lstdc++ -o app
