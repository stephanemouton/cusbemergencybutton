#!/bin/bash
for dev in /sys/bus/usb/devices/*-*:*
do
  if [ -f $dev/bInterfaceClass ]
  then
    if [[ "$(cat $dev/bInterfaceClass)" == "03" && "$(cat $dev/bInterfaceProtocol)" == "01" ]]
    then
      echo "Keyboard detected: $dev"
    fi
  fi
done

