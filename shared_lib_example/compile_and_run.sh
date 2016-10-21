#!/bin/bash

#Get the current absolute path
CUR_PATH=$(readlink -m .)

echo "Info:Remove all temporary files"
cd $CUR_PATH
rm -f main shared static *.o *.so *.a

#Catch errors and halt
set -e

echo "Info:Compile the Shared library"
gcc -fPIC -c -Wall -Werror -fpic shared.c -o shared.o
gcc -shared -o libshared.so shared.o

echo "Info:Compile the Static library"
gcc -c -Wall -Werror  -o static.o static.c
ar rc libstatic.a static.o

echo "Info:Compile Main"
gcc -Wall -Werror  main.c -L. -lshared -lstatic  -o main

echo "Info:Set LD Library Path"
export LD_LIBRARY_PATH=$CUR_PATH

echo "Info:Run main"
chmod +x main; ./main


