#!/bin/bash

sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install build-essential
sudo apt-get -y install ia32-libs

echo ""
echo " --- [Compiling and Linking] ---"
echo ""

#This fails b/c 32 vs 64 bit:
# g++ -I. -L. -lfmodex main.cpp

g++ -m32 -I. -L. -lfmodex main.cpp
