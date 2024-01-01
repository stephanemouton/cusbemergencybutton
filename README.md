# C USB emergency button/keyboard management

## Summary

The **usbemergencybutton** _daemon_ allows to use USB keyboards, sometimes very specific ones with a single key like the emergency button below, to trigger a script without being logged. It is a tool targeted to launch actions on a Linux machine in case of emergency.

![USB emergency button](./emergency_button.jpg)

*Example of USB button, defined by Vendor ID 0x2704 & Product ID 0x2018. See [tools](./tools/README.md) for more information.*

A script is associated to a button press using configuration file and is launched synchronously or in background.

## Hardware

Every keyboards or buttons, considered as such by Linux Kernel and therefore identified as input devices, can be used with usbemergencybutton.

A device usable with usbemergencybutton is listed as keyboard in `/proc/bus/input/devices`. The Vendor ID and Product ID of such device is used 

More information on how to retrieve those ID values with a shell script as well as examples of usable hardware is available in the [tools directory](./tools/README.md).

## Build & installation
### Dependencies
The iniparser library [written by Nicolas Devilla](http://ndevilla.free.fr/iniparser/) must be installed to run.

Development version of libiniparser needs also to be deployed in order to buld usbemergency (linker directive -l iniparser is used during build)

### Build
To compile usbemergencybutton, just type

`make -f Makefile.written`

The resulting executable is in the build directory.

### Deployment instructions
Install in /usr/local/bin using Makefile

`sudo make -f Makefile.written install`

## How to use

Launch the daemon at startup (systemd file provided) after having prepared a configuration file.
Default location of configuration file is /etc/usbemergency.conf
This value can be overridden with `-f` parameters (see below).
In *daemon* mode only one instance of usbemergencybutton is able to run. It is enforced by use of `/var/run/usbemergencybutton.pid` file containing the PID value. This rule does not apply when instances are launched in debug mode (with the `-d` parameter see below) and no PID is created in this case.

### Behaviour on POSIX signals

If modified, the configuration file can be read again without restarting the service by sending the `SIGHUP` signal. **Warning**: when usbemergencybutton is launched as a *daemon* and the location of the configuration file is specified as relative value, the program won't be able te reload the file since the service is detached from any console and therefore its default path is root (`/`) directory. It is not a bug but a feature. 

Normal termination of usbemergencybutton is expected with `SIGTERM` signal. In this case, the PID file is removed.

It is also possible to cleanly end usbemergencybutton with `SIGINT` signal but as the program is idle waiting for input, the device key must be pressed a last time in order to stop. PID file is also cleaned.

Finally, nothing can stop SIGQUIT and therefore the PID file is not cleaned when using this signal.

## Parameters

The daemon can be launched without parameters because default values are defined. However, the following parameters are available

* `-h` : print help and exit
* `-d` : debug mode. Launch program in display mode (remain connected to console) without PID file (`/var/run/usbemergencybutton.pid`) being created.
* `-f FILE` set name of configuration file (default is `/etc/usbemergencybutton.conf`)
* `-g`: generate example configuration file and halt

## Configuration file

The configuration file follow structure of .INI files
 
A sample of configuration file is provided: see [usbemergency.conf.example](./usbemergency.conf.example)

### Settings of the configuration file

The configuration file is divided into two sections : the `[Device]` and the `[Action]` one.
false]
**[Device]** section contains _daemons'_ settings.
* `vendor_ID =`: vendor ID of USB device, expressed in hexadecimal, as shown in /proc/bus/input/devices
* `product_ID =`: product ID of USB device, expressed in hexadecimal, as shown in /proc/bus/input/devices
* `expect_specific_key = [true|;` : Do we expect a specific key or any key stroke is valid? 
* `key_value =`: Code of the expected key, expressed in hexadecimal, if detection is activated. See [tools](./tools/README.md) directory to know how to find such value.

**[Action]** section gathers definition of action to execute on button press.
* `asynchronous_launch = [false|true];` Script to be launched in asynchronous mode (i.e. in background)? Default is false.
* `script =`: Script launched when the button is pressed. Default `/bin/true`

## TO-DO list

In the roadmap for next versions:
* Generate Makefile and compile using Autotools
* Write man page
* Add more automated tests
* Add support for translation
* package as RPM

## Related documentation

The following links contain information used to design and code usbemergency and [associated script tools ](./tools/README.md).

* ["Linux Keylogger: How to Read the Linux Keyboard Buffer"](https://www.codyhiar.com/blog/linux-keylogger-how-to-read-linux-keyboard-buffer/). Extensive information on low level, close to the hardware, information available from input devices. Also contains hints used to write `list_event_devices.sh` described in [tools](./tools/README.md) directory.  
* ["Accessing Keys from Linux Input Device"](
https://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device). Details how to get keyed values in low level events input method.
* ["Is it possible for a daemon (i.e. background) process to look for key-presses from a USB keyboard?"](https://unix.stackexchange.com/questions/94322/is-it-possible-for-a-daemon-i-e-background-process-to-look-for-key-presses-fr/94329#94329). Describes how to get low level events from a keyboard for a non interactive program. Also contains details on how to spot low level input events from a keyboard (hint: event flags array EV == 120013).
* ["Get /dev/input/eventX from an attached USB device with PID:VID"](https://stackoverflow.com/questions/7681962/linux-get-dev-input-eventx-from-an-attached-usb-device-with-pidvid). Inspiration and hints to get input from a device with known Vendor and Product IDs. 
* ["Determine Linux keyboard events device"](https://stackoverflow.com/questions/29678011/determine-linux-keyboard-events-device). Extensively explains how to get information of event device number both by scripts and with C code.

## License
This software is released under MIT License. See LICENSE for full informations
