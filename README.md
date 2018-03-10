# ESPer - Space Automation Firmware for ESP8266

ESPer is a firmware for ESP8266 based IoT systems.
IT provides a common platform for all kind of devices and establishes a common communication interface.

ESPer uses allows to implement firmware for classes of devices and let each of the device expose device-specific features.
The features expose their current status and their controllable functions over a unified [mqtt](http://mqtt.org/) interface and allows easy integration in home-automation controllers.
For easy management, devices publish current status information and can be updated over the WiFi interface.


## Requirements

For build, ESPer requires [Sming](https://github.com/SmingHub/Sming) to be installed. We recommend using our Docker Image (`esperhub/esper`), which already contains all required dependencies.

During runtime, a 2.4GHz WiFi Network with WPA or WPA2 encryption is required.
It is highly recommended to use a separated network for ESPer based devices to limit the broadcast and multicast traffic.
In addition, a MQTT server is required.

Depending on the features used, some services must be provided in the environment during runtime:
* For OTA Updates, a Web-Server capable of serving files via HTTP is required.
* For Heartbeat, a timer service must be used to send the heartbeat messages.


## Getting Started

### Create a site directory
Create a project directory and initialize it as `git` repository:
~~~~
mkdir my-site
cd my-site
git init
~~~~

### Add ESPer Repository
Add the ESPer framework repository as submodule:
~~~~
git submodule add https://github.com/esper-hub/esper.git
~~~~

### Create environment configuration
Then copy `esper/Configuration.mx.example` to `./Configuration.mk` and change it according to the target environment.
This config file contains your environment specific settings like WiFi credentials and Servers to use.
Take some time to walk through the available options.

### Create a device implementation
Inside the project-directory, create a subdirectory called `devices/mydev` and add your device specific code here.
The code must implement a function `Device* createDevice()` which returns a device instance. The esper repository
has several examples you can draw inspiration from.

### Build the firmware
Execute the makefile in the ESPer directory while specifying esper and site directory:
~~~~
docker run -it --rm --volume /home/user/esper-site:/home/builder/site --volume /home/user/esper-site/esper:/home/builder/esper esperhub/esper:latest
~~~~
The resulting images will be in `/home/user/esper-site/esper/dist`.

### Flash the firmware
Connect the ESP to your computer and boot it into the flash mode. Then execute the following command:
~~~~
docker run -it --rm --volume /home/user/esper-site:/home/builder/site --volume /home/user/esper-site/esper:/home/builder/esper esperhub/esper:latest make mydev/flash
~~~~
Once flashed the container will attach to the ESPs terminal.

