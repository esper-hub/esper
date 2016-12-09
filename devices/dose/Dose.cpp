#include "../features/Socket.h"
#include "Device.h"


constexpr const char PLUG_NAME[] = "socket";
constexpr const uint16_t PLUG_GPIO = 2;


class SocketFeature : public Feature<PLUG_NAME> {
    using Feature<PLUG_NAME>::LOG;

    static const String ON;
    static const String OFF;

public:
    SocketFeature(Device* device) :
            Feature<PLUG_NAME>(device),
            state(false) {
        pinMode(PLUG_GPIO, INPUT);

        LOG.log("Initialized");
    }

    void set(bool state) {
        if (this->state = state) {
            LOG.log("Turning on");
            pinMode(PLUG_GPIO, OUTPUT);

        } else {
            LOG.log("Turning off");
            pinMode(PLUG_GPIO, INPUT);
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


const String SocketFeature::ON = "1";
const String SocketFeature::OFF = "0";



class DoseDevice : public Device {
public:
    DoseDevice() :
            socket(this) {
        this->add(&(this->socket));
    }

private:
    SocketFeature socket;
};



Device* createDevice() {
    return new DoseDevice();
}
