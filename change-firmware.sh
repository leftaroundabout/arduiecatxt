#!/bin/bash

argument="$1"
firmwares="$(dirname "$(readlink -f "$0")")/USBKeyboard/firmware"
dfuprogrammercmd="sudo dfu-programmer atmega16u2"

function chfirmware() {
    $dfuprogrammercmd erase
    $dfuprogrammercmd ${firmwares}/$1.hex
    $dfuprogrammercmd reset
}

if [[ -z $argument ]] ; then
  echo "Available options:"
  echo "d[efault]  - the standard firmware, allows uploading snippets with e.g. IDE"
  echo "k[eyboard] - keyboard-HID firmware. For actually using the device."
  exit 1
else
  echo "This scripts assumes you have an Arduino Uno connected and the USB controller reset."
  echo "See https://www.arduino.cc/en/Hacking/DFUProgramming8U2 for info."
  set -e
  case $argument in
   d|default)
    chfirmware "Arduino-usbserial-uno"
    echo "Changed firmware to Arduino-usbserial."
    ;;
   k|keyboard)
    chfirmware "Arduino-keyboard-0.3"
    echo "Changed firmware to Arduino-keyboard."
    ;;
  esac
fi
