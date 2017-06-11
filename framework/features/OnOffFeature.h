#ifndef ONOFF_H
#define ONOFF_H

#include "Feature.h"


template<const char* const name, uint16_t gpio, bool invert = false, uint16_t damper = 0>
class OnOffFeature : public Feature<name> {
    constexpr static const char* const ON = "1";
    constexpr static const char* const OFF = "0";

protected:
    using Feature<name>::LOG;

public:
    OnOffFeature(Device* const device, bool initial_state = false) :
            Feature<name>(device),
            state(initial_state),
            lastChange(RTC.getRtcSeconds()) {
        pinMode(gpio, OUTPUT);
        digitalWrite(gpio, this->state == !invert);

        this->registerSubscription("set", Device::MessageCallback(&OnOffFeature::onMessageReceived, this));

        LOG.log("Initialized");
    }

    void set(bool state) {
        if (this->state = state) {
            LOG.log("Turning on");
        } else {
            LOG.log("Turning off");
        }

        digitalWrite(gpio, state == !invert);

        this->publishCurrentState();
    }

protected:
    virtual void publishCurrentState() {
        this->publish("", this->state ? ON : OFF, true);
    }

private:
    void onMessageReceived(const String& topic, const String& message) {
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
