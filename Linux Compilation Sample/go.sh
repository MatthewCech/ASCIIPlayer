#!/bin/bash

sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install build-essential
sudo apt-get -y install ia32-libs

echo ""
echo ""
echo ""
echo "_____[ Running g++ now ]_____"
echo ""
echo ""
echo ""

g++ -std=c++14 -m32 -march=i386 -I./ -L./ -l fmodexL AudioSystem.cpp main.cpp -v
