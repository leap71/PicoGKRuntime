#!/bin/sh

# Install the packages needed to build the PicoGK Runtime on Debian/Ubuntu
# (tested on Ubuntu 26.04, x86-64, GCC 15)

sudo apt-get update

# Build tools
sudo apt-get install -y build-essential cmake ninja-build git pkg-config

# OpenVDB dependencies
sudo apt-get install -y libboost-iostreams-dev libboost-dev libtbb-dev libblosc-dev zlib1g-dev \
    libzstd-dev liblz4-dev

# GLFW (X11 and Wayland) and OpenGL headers for the viewer
sudo apt-get install -y libx11-dev libxrandr-dev libxinerama-dev libxcursor-dev libxi-dev \
    libxkbcommon-dev libgl-dev libwayland-dev wayland-protocols
