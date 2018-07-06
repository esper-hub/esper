#include "../features/Socket.h"
#include "../features/TriggerButton.h"
#include "Device.h"


constexpr const char SOCKET_NAME[] = "socket";
constexpr const uint16_t SOCKET_GPIO = 12;

constexpr const char BUTTON_NAME[] = "button";
constexpr const uint16_t BUTTON_GPIO = 0;


class SonoffDevice : public Device {
public:
    SonoffDevice() :
            socket(this),
            button(this, decltype(button)::Callback(&SonoffDevice::onStateChanged, this)) {
        this->add(&(this->socket));
        this->add(&(this->button));
    }

private:
    void onStateChanged(const bool& state) {
        this->socket.toggle();
    }

    Socket<SOCKET_NAME, SOCKET_GPIO> socket;
    TriggerButton<BUTTON_NAME, BUTTON_GPIO, true> button;
};


Device* createDevice() {
    return new SonoffDevice();
}
