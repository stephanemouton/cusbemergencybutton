#!/bin/bash
# List devices identified as keyboards (i.e. with EV flags of 120013)
echo "-- List devices identified as keyboards (EV=120013) in /proc/bus/input/devices --"
echo "Please notice:"
echo " - Non USB devices (ex : laptop keyboard) also appear in the list"
echo " - Vendor ID and product ID are the values to use in configuration file"
grep -E 'Handlers|EV=|Bus=' /proc/bus/input/devices | grep -B2 'EV=120013' | grep -v -E 'EV=|--'

