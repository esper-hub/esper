#include <user_config.h>

#include "devices/HassDevice.h"


extern HassDevice* device;


void init() {
    device->start();
}
