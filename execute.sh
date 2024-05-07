#!/bin/sh

cd /home/leejaein/Documents/pintos/threads
make

cd build
chmod +x ../../utils/pintos

../../utils/pintos automated_warehouse 5 2A:4C:2B:2C:3A
#../../utils/pintos automated_warehouse 16 2A:5C:1B:3A:6B:4A:7C:1A:5B:3C:4A:2B:6C:7A:4B:5A