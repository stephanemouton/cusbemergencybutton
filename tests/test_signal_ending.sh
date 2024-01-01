#!/bin/sh
configuration_file=./usbemergencybutton.conf.signal
pid_file=/var/run/usbemergencybutton.pid
echo '# Testing signal handling on SIGTERM: program should terminate quietly and remove PID'
echo 'Expecting emergency button with IDs:Vendor=2704 Product=2018'
../build/usbemergencybutton -f $configuration_file
sleep 3
pid_value=`cat $pid_file`
kill_command="kill -TERM $pid_value"
echo $kill_command
$kill_command
sleep 3
if test -f "$pid_file"; then
    echo "$pid_file still exists."
    echo "test FAILED"
    exit 1
else
    echo "$pid_file removed before program ends."
    echo "Test SUCCEEDED"
fi
