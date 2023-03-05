# C USB emergency button/keyboard management #

## Summary ##

The usbpemergency _daemon_ allows to use USB keyboards, sometimes very specific ones with a single key like the emergency button below, to trigger a script without being logged. 
It is therefore a tool to launch actions on a Linux machine in case of emergency.


 identified by their Vendor ID & Product_ID


![USB emergency button](./emergency_button.jpg)

A script is associated to a button press using configuration file and is launched synchronously or in background.

## Build & installation ##
### Dependencies ###
The iniparser library [written by Nicolas Devilla](http://ndevilla.free.fr/iniparser/) must be installed to run.

Development version of libiniparser needs also to be deployed in order to buld usbemergency (directive -l iniparser is used during build)


### Build ###
To compile usbemergency, just type

`make -f Makefile.written`

The resulting executable is in the build directory.

### Deployment instructions ###
Install in /usr/local/bin using Makefile

`make -f Makefile.written install`

## How to use ##

Launch the daemon at startup (systemd file provided) after having prepared a configuration file

Default location of configuration file is /etc/usbemergency.conf

This value can be overridden with -f parameters

## Parameters ## 

To do

## Configuration ##

The configuration file follow structure of .INI files
 
An sample of configuration file is provided: see [usbemergency.conf.example](./usbemergency.conf.example)

### Parameters ###

The configuration file is divided into two sections : the Global and the Action one.

**[Device]** section contains _daemons'_ settings

to do

**[Action]** section gathers definition of action to execute on button press

to do
 
Please note that it is not possible to reliably guess buttons number before plugin and testing them.   

### Related projects and documentation ###

Keyboard action
https://stackoverflow.com/questions/20943322/accessing-keys-from-linux-input-device
https://www.codyhiar.com/blog/linux-keylogger-how-to-read-linux-keyboard-buffer/

Detecting keyboard
https://askubuntu.com/questions/725068/where-is-my-keyboard-mouse-and-other-hci-devices-mounted-in-linux
https://serverfault.com/questions/126291/ubuntu-keyboard-detection-from-bash-script


## License ##
This software is released under MIT License. See LICENSE for full informations
