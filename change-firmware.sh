#!/bin/bash

argument="$1"
firmwares="$(dirname "$(readlink -f "$0")")/USBKeyboard/firmware"
dfuprogrammercmd="sudo dfu-programmer atmega16u2"

if [[ -z $argument ]] ; then
  echo "Available options:"
  echo "d[efault] - the standard firmware, allows uploading snippets with e.g. IDE"
  exit 1
else
  echo "This scripts assumes you have an Arduino Uno connected and the USB controller reset."
  echo "See https://www.arduino.cc/en/Hacking/DFUProgramming8U2 for info."
  set -e
  case $argument in
   d|default)
    $dfuprogrammercmd erase
    $dfuprogrammercmd ${firmwares}/Arduino-usbserial-uno.hex
    $dfuprogrammercmd reset
    echo "Changed firmware to Arduino-usbserial."
    ;;
  esac
fi
