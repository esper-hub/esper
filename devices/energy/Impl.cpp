#include "Device.h"
#include "Pulse.h"


constexpr const char PULSE_NAME[] = "pulse";
constexpr const uint16_t PULSE_GPIO = 2;


class EnergyDevice : public Device {
public:
    EnergyDevice() :
            pulse(this) {
        this->add(&(this->pulse));
    }

private:
    PulseFeature<PULSE_NAME, PULSE_GPIO, 1> pulse;
};


Device* createDevice() {
    return new EnergyDevice();
}
