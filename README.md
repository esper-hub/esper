# ESPer - Space Automation Firmware for ESP8266

ESPer is a firmware for ESP8266 based IoT systems.
IT provides a common platform for all kind of devices and establishes a common communication interface.

ESPer uses allows to implement firmware for classes of devices and let each of the device expose device-specific features.
The features expose their current status and their controllable functions over a unified [mqtt](http://mqtt.org/) interface and allows easy integration in home-automation controllers.
For easy management, devices publish current status information and can be updated over the WiFi interface.


## Requirements

For build, ESPer requires [Sming](https://github.com/SmingHub/Sming) to be installed.

During runtime, a 2.4GHz WiFi Network with WPA or WPA2 encryption is required.
It is highly recommended to use a separated network for ESPer based devices to limit the broadcast and multicast traffic.
In addition, a MQTT server is required.

Depending on the features used, some services must be provided in the environment during runtime:
* For OTA Updates, a Web-Server capable of serving files via HTTP is required.
* For Heartbeat, a timer service must be used to send the heartbeat messages.


## Getting Started

### Install esp-open-sdk and Sming
See [their README](https://github.com/SmingHub/Sming/blob/develop/Readme.md) for further details.

### Clone the ESPer Repository
~~~~
git clone https://git.maglab.space/esper/esper.git
~~~~

### Create a site directory
Create a directory beside the ESPer repository.
Then copy `Configuration.mx.example` to `Configuration.mk` and change according to the target environment.

### Create a device implementation
Inside the the site-directory create a subdirectory called `devices/mydev` and add your device specific code here.
The code must implement a function `Device* createDevice()` which returns a device instance.

### Build the firmware
Execute the makefile in the ESPer directory while specifying the site directory:
~~~~
make SITE=path/to/site
~~~~

### Flash the firmware
Boot the ESP into the flash mode and execute the following command:
~~~~
make SITE=path/to/site mydev/flash
~~~~
