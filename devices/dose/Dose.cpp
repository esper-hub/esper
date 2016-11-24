#include "../features/Socket.h"
#include "Device.h"


class DoseDevice : public Device {
    constexpr static const char PLUG1_NAME[] = "sw1";
    constexpr static const uint16_t PLUG1_GPIO = 12;

    constexpr static const char PLUG2_NAME[] = "sw2";
    constexpr static const uint16_t PLUG2_GPIO = 14;

public:
    DoseDevice() :
            socket1(this),
            socket2(this) {
        this->add(&(this->socket1));
        this->add(&(this->socket2));
    }

private:
    Socket<PLUG1_NAME, PLUG1_GPIO> socket1;
    Socket<PLUG2_NAME, PLUG1_GPIO> socket2;
};


constexpr const char DoseDevice::PLUG1_NAME[];
constexpr const uint16_t DoseDevice::PLUG1_GPIO;

constexpr const char DoseDevice::PLUG2_NAME[];
constexpr const uint16_t DoseDevice::PLUG2_GPIO;



Device* createDevice() {
    return new DoseDevice();
}
