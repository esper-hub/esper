#ifndef ONOFF_H
#define ONOFF_H

#include "Feature.h"


template<const char* name, uint16_t gpio, bool invert = false, int damper = 0>
class OnOffFeature : public Feature<name> {
    using Feature<name>::LOG;

    constexpr static const char* const ON = "1";
    constexpr static const char* const OFF = "0";

public:
    OnOffFeature(Device* device, bool initial_state = false) :
            Feature<name>(device),
            state(initial_state),
            lastChange(RTC.getRtcSeconds()) {
        pinMode(gpio, OUTPUT);

        LOG.log("Initialized.");
    }

    inline void set(bool state) {
        if (this->state = state) {
            LOG.log("Turning on");
        } else {
            LOG.log("Turning off");
        }

        digitalWrite(gpio, state == !invert);

        this->publishCurrentState();
    }

protected:
    virtual void registerSubscriptions() {
        this->registerSubscription("set", Device::MessageCallback(&OnOffFeature::onMessageReceived, this));
    }

    inline void publishCurrentState() {
        this->publish("state", this->state ? ON : OFF);
    }

private:
    virtual void onMessageReceived(const String& topic, const String& message) {
        const uint32_t now = RTC.getRtcSeconds();

        if (damper > 0) {
            if (this->lastChange + damper > now)
                return;
        }

        if (message == ON) {
            this->set(true);

        } else if (message == OFF) {
            this->set(false);

        } else {
            LOG.log("Unknown message received:", message);
        }

        this->lastChange = now;
    }

    bool state;

    uint32_t lastChange;
};

#endif
