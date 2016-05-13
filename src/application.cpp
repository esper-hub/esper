#include <user_config.h>
#include "device.h"
#include "devices/EspDose.h"


class EspSchalter : public Device {
public:
    EspSchalter() : Device("FIXME") { }
};


void init() {

    Device *device;

    constexpr static const DeviceType device_type = DEVICE_TYPE;

    switch (device_type) {
        case DeviceType::ESP_DOSE:
            device = new EspDose();
            break;
        case DeviceType::ESP_SCHALTER:
            device = new EspSchalter();
            break;
    }

    device->start();
}
