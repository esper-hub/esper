#include "../features/Socket.h"
#include "Device.h"


template<const char* name, uint16_t gpio_raise, uint16_t gpio_lower>
class ScreenFeature : public Feature<name> {
    using Feature<name>::LOG;

    constexpr static const char* const RAISE = "RAISE";
    constexpr static const char* const LOWER = "LOWER";

    constexpr static const uint16_t CYCLE_HI = 100;
    constexpr static const uint16_t CYCLE_LO = 100;

public:
    ScreenFeature(Device* device) :
            Feature<name>(device) {
        pinMode(gpio_raise, OUTPUT);
        pinMode(gpio_lower, OUTPUT);

        LOG.log("Initialized");
    }

protected:
    virtual void registerSubscriptions() {
        this->registerSubscription("set", Device::MessageCallback(&ScreenFeature::onMessageReceived, this));
    }

    virtual void publishCurrentState() {
        // This is a NO-OP for now, as we don't have any feedback from the controlling device
    }

private:
    void onMessageReceived(const String& topic, const String& message) {
        uint16_t gpio;
        if (message == RAISE) {
            gpio = gpio_raise;

        } else if (message == LOWER) {
            gpio = gpio_lower;

        } else {
            LOG.log("Illegal command:", message);
            return;
        }

        digitalWrite(gpio, true);
        delayMilliseconds(CYCLE_HI);
        digitalWrite(gpio, false);
        delayMilliseconds(CYCLE_LO);
        digitalWrite(gpio, true);
        delayMilliseconds(CYCLE_HI);
        digitalWrite(gpio, false);
    }
};


class ScreenDevice : public Device {
    constexpr static const char SCREEN_NAME[] = "screen";

    constexpr static const uint16_t SCREEN_GPIO_RAISE = 0;
    constexpr static const uint16_t SCREEN_GPIO_LOWER = 1;


public:
    ScreenDevice() :
            screen(this) {
        this->add(&(this->screen));
    }

private:
    ScreenFeature<SCREEN_NAME, SCREEN_GPIO_RAISE, SCREEN_GPIO_LOWER> screen;
};


constexpr const char ScreenDevice::SCREEN_NAME[];
constexpr const uint16_t ScreenDevice::SCREEN_GPIO_RAISE;
constexpr const uint16_t ScreenDevice::SCREEN_GPIO_LOWER;


Device* createDevice() {
    return new ScreenDevice();
}
