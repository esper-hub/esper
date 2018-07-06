#include "../features/Socket.h"
#include "../features/ToggleButton.h"
#include "../features/Light.h"
#include "Device.h"


constexpr const char SOCKET_NAME[] = "socket";
constexpr const uint16_t SOCKET_GPIO = 12;

constexpr const char BUTTON_NAME[] = "button";
constexpr const uint16_t BUTTON_GPIO = 0;

class SonoffT1Device : public Device {
public:
    SonoffT1Device() :
            socket(this),
            button(this, decltype(button)::Callback(&SonoffT1Device::onStateChanged, this)) {
        this->add(&(this->socket));
        this->add(&(this->button));
    }

private:
    void onStateChanged(const bool& state) {
        this->socket.set(state);
    }

    Socket<SOCKET_NAME, SOCKET_GPIO> socket;
    ToggleButton<BUTTON_NAME, BUTTON_GPIO, true> button;
};


Device* createDevice() {
    return new SonoffT1Device();
}
