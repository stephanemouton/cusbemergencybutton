#!/bin/sh
configuration_file=./usbemergencybutton.conf.signal
pid_file=/var/run/usbemergencybutton.pid
echo '# Testing signal handling on SIGINT: program should terminate quietly AFTER a last press on device'
echo 'Expecting emergency button with IDs:Vendor=2704 Product=2018'
../build/usbemergencybutton -f $configuration_file
sleep 3
pid_value=`cat $pid_file`
kill_command="kill -INT $pid_value"
echo $kill_command
$kill_command
echo 'Please press button in the following 5 seconds'
sleep 5
if test -f "$pid_file"; then
    echo "$pid_file still exists."
    echo "Did you press button?"
    exit 1
else
    echo "$pid_file removed before program ends."
    echo "Test SUCCEEDED"
fi
