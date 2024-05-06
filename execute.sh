#!/bin/sh

cd /home/leejaein/Documents/pintos/threads
make

cd build
chmod +x ../../utils/pintos

#../../utils/pintos automated_warehouse 5 2A:4C:2B:2C:3A
../../utils/pintos automated_warehouse 4 2A:4C:2B:2C