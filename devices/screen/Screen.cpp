#include "../features/Socket.h"
#include "Device.h"


constexpr const char SCREEN_NAME[] = "screen";


class ScreenFeature : public Feature<SCREEN_NAME> {
    using Feature<SCREEN_NAME>::LOG;

    constexpr static const uint16_t GPIO_RAISE = 0;
    constexpr static const uint16_t GPIO_LOWER = 2;

    constexpr static const char* const RAISE = "RAISE";
    constexpr static const char* const LOWER = "LOWER";

    constexpr static const uint16_t CYCLE_HI = 300;
    constexpr static const uint16_t CYCLE_LO = 500;

public:
    ScreenFeature(Device* device) :
            Feature<SCREEN_NAME>(device) {
        pinMode(GPIO_RAISE, OUTPUT);
        pinMode(GPIO_LOWER, OUTPUT);

        digitalWrite(GPIO_RAISE, true);
        digitalWrite(GPIO_LOWER, true);

        LOG.log("Initialized");
    }

protected:
    virtual void registerSubscriptions() {
        this->registerSubscription("cmd", Device::MessageCallback(&ScreenFeature::onMessageReceived, this));
    }

    virtual void publishCurrentState() {
        // This is a NO-OP for now, as we don't have any feedback from the controlling device
    }

private:
    void onMessageReceived(const String& topic, const String& message) {
        uint16_t gpio;
        if (message == RAISE) {
            LOG.log("Raise screen");
            gpio = GPIO_RAISE;

        } else if (message == LOWER) {
            LOG.log("Lower screen");
            gpio = GPIO_LOWER;

        } else {
            LOG.log("Illegal command:", message);
            return;
        }

        digitalWrite(gpio, false);
        delayMilliseconds(CYCLE_HI);
        digitalWrite(gpio, true);

        delayMilliseconds(CYCLE_LO);

        digitalWrite(gpio, false);
        delayMilliseconds(CYCLE_HI);
        digitalWrite(gpio, true);
    }
};


class ScreenDevice : public Device {
public:
    ScreenDevice() :
            screen(this) {
        this->add(&(this->screen));
    }

private:
    ScreenFeature screen;
};


Device* createDevice() {
    return new ScreenDevice();
}
