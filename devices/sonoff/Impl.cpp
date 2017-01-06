#include "../features/Socket.h"
#include "../features/Button.h"
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
            button(this),
            light(this) {
        this->add(&(this->socket));
        this->add(&(this->button));
        this->add(&(this->light));
    }

private:
    OnOffFeature<SOCKET_NAME, SOCKET_GPIO, false, 1> socket;
    Button<BUTTON_NAME, BUTTON_GPIO, true> button;
    Light<LIGHT_NAME, LIGHT_GPIO> light;
};


Device* createDevice() {
    return new SonoffDevice();
}
