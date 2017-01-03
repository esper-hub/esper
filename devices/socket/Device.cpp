#include "../features/Socket.h"
#include "Device.h"


constexpr const char SOCKET_NAME[] = "socket";
constexpr const uint16_t SOCKET_GPIO = 2;


class SocketFeature : public Feature<SOCKET_NAME> {
    using Feature<SOCKET_NAME>::LOG;

    constexpr static const char* const ON = "1";
    constexpr static const char* const OFF = "0";

public:
    SocketFeature(Device* device) :
            Feature<SOCKET_NAME>(device),
            state(false) {
        pinMode(SOCKET_GPIO, INPUT);

        LOG.log("Initialized");
    }

    void set(bool state) {
        if (this->state = state) {
            LOG.log("Turning on");
            pinMode(SOCKET_GPIO, OUTPUT);

        } else {
            LOG.log("Turning off");
            pinMode(SOCKET_GPIO, INPUT);
        }

        this->publishCurrentState();
    }

protected:
    virtual void registerSubscriptions() {
        this->registerSubscription("set", Device::MessageCallback(&SocketFeature::onMessageReceived, this));
    }

    virtual void publishCurrentState() {
        this->publish("state", this->state ? ON : OFF);
    }

private:
    void onMessageReceived(const String& topic, const String& message) {
        const uint32_t now = RTC.getRtcSeconds();

        if (message == ON) {
            this->set(true);

        } else if (message == OFF) {
            this->set(false);

        } else {
            LOG.log("Unknown message received:", message);
        }
    }

    bool state;
};



class SocketDevice : public Device {
public:
    SocketDevice() :
            socket(this) {
        this->add(&(this->socket));
    }

private:
    SocketFeature socket;
};



Device* createDevice() {
    return new SocketDevice();
}
