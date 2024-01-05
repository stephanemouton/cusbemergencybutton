#!/bin/sh
configuration_file=./usbemergencybutton.conf.script
result_script=/tmp/result_usbemergencybutton_script.log
pid_file=/var/run/usbemergencybutton.pid
echo '# Testing script execution on key press'
echo 'Expecting emergency button with IDs:Vendor=0x05a4 Product=0x9759'
../build/usbemergencybutton -f $configuration_file
sleep 3
echo 'Please press button within 5 seconds'
sleep 5
echo 'Checking result'
pid_value=`cat $pid_file`
kill_command="kill -TERM $pid_value"
$kill_command
if test -f "$result_script"; then
    echo "$result_script exists."
    echo "test PASSED"
    #rm -f $result_script
else
    echo "test FAILED"
    exit 1
fi

