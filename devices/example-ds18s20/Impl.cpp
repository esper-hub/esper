#include "../features/DS18S20.h"
#include "Device.h"


constexpr const char DS18S20_NAME[] = "ds18s20";
const uint8_t DS18S20_GPIO = 0;

class DS18S20Device : public Device {
public:
    DS18S20Device() :
            ds18s20(this)
    {
        this->add(&(this->ds18s20));
    }

private:
    DS18S20Sensor<DS18S20_NAME, DS18S20_GPIO> ds18s20;
};


Device* createDevice() {
    return new DS18S20Device();
}


