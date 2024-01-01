#!/bin/sh
configuration_file=./usbemergencybutton.conf.keypad
echo '# Testing other device, keypad, in debug mode'
echo "Expecting keypad device defined in $configuration_file"
../build/usbemergencybutton -d -f $configuration_file
