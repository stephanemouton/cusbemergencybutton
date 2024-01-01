#!/bin/sh
configuration_file=./usbemergencybutton.conf.signal
pid_file=/var/run/usbemergencybutton.pid
echo '# Testing PID file creation'
echo 'Expecting emergency button with IDs:Vendor=2704 Product=2018'
../build/usbemergencybutton -f $configuration_file
sleep 3
if test -f "$pid_file"; then
    echo "$pid_file exists."
else
    echo "test FAILED"
    exit 1
fi

