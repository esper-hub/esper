#ifndef CYBERHOEHLE_DEVICE_H
#define CYBERHOEHLE_DEVICE_H

#include "HassDevice.h"

class Device {
    HassDevice *device;
public:
    Device(const char *name) : device(new HassDevice(name)) { }

    inline HassDevice *getDevice() const {
        return device;
    }

    inline void start() {
        device->start();
    }
};

#endif //CYBERHOEHLE_DEVICE_H
