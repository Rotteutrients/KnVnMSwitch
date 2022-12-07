#!/bin/bash
strstr() {
  [ "${1#*$2*}" = "$1" ] && return 1
  return 0
}

set -e # Exit on error

export PICO_SDK_PATH="$(cd ../../pico-sdk/; pwd)"
strstr $PATH $(cd ../../arm-none-eabi/bin/; pwd) || export PATH="$(cd ../../arm-none-eabi/bin/; pwd):$PATH"

rm -rf build
mkdir build
cd build
cmake ..
make -j