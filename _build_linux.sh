#!/bin/bash
#Update and upgrade the system, verify gcc build essentials is installed. The primary linux limitation is that the library files are 32-bit only, and can only be compiled with -m32 / -melf_i386 compatibility settings. Adding --verbose to `ld` seems to help with debugging.

sudo apt-get -y update
sudo apt-get -y upgrade
sudo apt-get -y install build-essential
sudo apt-get -y install ia32-libs

Build/run_clean.sh
Build/run_premake_gmake.sh
Build/run_make_gmake.sh
