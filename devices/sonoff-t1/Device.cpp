#include <esper/features/Socket.h>
#include <esper/features/TriggerButton.h>
#include <esper/Device.h>


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
        if (state) {
            this->socket.toggle();
        }
    }

    Socket<SOCKET_NAME, SOCKET_GPIO> socket;
    TriggerButton<BUTTON_NAME, BUTTON_GPIO, true> button;
};


Device* createDevice() {
    return new SonoffT1Device();
}
