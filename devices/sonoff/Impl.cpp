#include "../features/Socket.h"
#include "Device.h"


constexpr const char SOCKET_NAME[] = "socket";
constexpr const uint16_t SOCKET_GPIO = 12;


class SonoffDevice : public Device {
public:
    SonoffDevice() :
            socket(this) {
        this->add(&(this->socket));
    }

private:
    OnOffFeature<SOCKET_NAME, SOCKET_GPIO, false, 1> socket;
};


Device* createDevice() {
    return new SonoffDevice();
}
