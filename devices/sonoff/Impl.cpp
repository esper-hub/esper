#include "../features/Socket.h"
#include "../features/ToggleButton.h"
#include "../features/Light.h"
#include "Device.h"


constexpr const char SOCKET_NAME[] = "socket";
constexpr const uint16_t SOCKET_GPIO = 12;

constexpr const char BUTTON_NAME[] = "button";
constexpr const uint16_t BUTTON_GPIO = 0;

constexpr const char LIGHT_NAME[] = "light";
constexpr const uint16_t LIGHT_GPIO = 13;


class SonoffDevice : public Device {
public:
    SonoffDevice() :
            socket(this),
            light(this),
            button(this, decltype(button)::Callback(&SonoffDevice::onStateChanged, this)) {
        this->add(&(this->socket));
        this->add(&(this->light));
        this->add(&(this->button));
    }

private:
    void onStateChanged(const bool& state) {
        this->socket.set(state);
        this->light.set(state);
    }

    Socket<SOCKET_NAME, SOCKET_GPIO> socket;
    Light<LIGHT_NAME, LIGHT_GPIO> light;
    ToggleButton<BUTTON_NAME, BUTTON_GPIO, true> button;
};


Device* createDevice() {
    return new SonoffDevice();
}
