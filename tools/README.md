# How to use a given USB device usbemergencybutton
## Tools to detect usable USB devices 
Several tools are provided to help find usable connected keyboards and set input device information (vendor ID and product IV) in usbemergency configuration file.
* `list_usb_dev.sh`: list all USB input devices, even not keyboards. Helpful to check if USB hardware is detected correctly. Exploit data from `/sys/bus/usb/devices/usb*/` according to suggestions on [how to detect keyboard from bash script](https://serverfault.com/questions/126291/ubuntu-keyboard-detection-from-bash-script) and [locate where they are mounted](https://askubuntu.com/questions/725068/where-is-my-keyboard-mouse-and-other-hci-devices-mounted-in-linux
) in sys pseudofilesystem.
* `list_usb_keyboards.sh `: list all USB Keyboards in the USB device tree. The result of the script confirms findings of the first script but does not directly provide vendor ID and product ID required to configure usbemergency.
* `list_event_devices.sh `: list information, vendor ID and product ID of all devices producing keyboard input events *including the non USB ones*. The two previous scripts allow to differentiate in the result list plugged USB devices.
* `get_key_from_device`: as its name suggests, once a device is selected the tool help identify a specific key code emitted by a device. It is useful when a given key is expected. For other cases and for single key buttons, the `expect_specific_key = false;` parameter of the configuration file avoids to provide a key identifier.  

## Example of usable USB devices

 * [USB emergency button](
https://www.aliexpress.com/item/1005003800699044.html?spm=a2g0o.search0304.0.0.6b47165eRqS1KD&algo_pvid=a37416ee-b6a4-455a-a2be-386e44153b9b&algo_exp_id=a37416ee-b6a4-455a-a2be-386e44153b9b-9&pdp_ext_f=%7B%22sku_id%22%3A%2212000027209131734%22%7D&pdp_npi=2%40dis%21EUR%21%2135.98%21%21%21%21%21%402103143616613329165173287e1060%2112000027209131734%21sea) (a.k.a. "Big Red Button").
 * [Configurable additional keyboard](
https://www.aliexpress.com/item/1005004652894780.html?spm=a2g0o.search0304.0.0.6b47165eRqS1KD&algo_pvid=a37416ee-b6a4-455a-a2be-386e44153b9b&algo_exp_id=a37416ee-b6a4-455a-a2be-386e44153b9b-13&pdp_ext_f=%7B%22sku_id%22%3A%2212000029989257141%22%7D&pdp_npi=2%40dis%21EUR%21%2117.72%21%21%21%21%21%402103143616613329165173287e1060%2112000029989257141%21sea).
 * [Foot pedal "keyboard"](
https://www.aliexpress.com/item/1005003140625903.html?spm=a2g0o.search0304.0.0.6b47165eRqS1KD&algo_pvid=a37416ee-b6a4-455a-a2be-386e44153b9b&algo_exp_id=a37416ee-b6a4-455a-a2be-386e44153b9b-19&pdp_ext_f=%7B%22sku_id%22%3A%2212000024311357831%22%7D&pdp_npi=2%40dis%21EUR%21%2121.9%21%21%21%21%21%402103143616613329165173287e1060%2112000024311357831%21sea).
