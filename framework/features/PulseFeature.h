#ifndef PULSEFEATURE_H
#define PULSEFEATURE_H

#include "Feature.h"
#include "../util/Damper.h"

template<const char* const name, uint16_t gpio, bool invert = false, uint16_t damper_time = 0, uint16_t max_duration = 0>
class PulseFeature : public Feature<name> {

protected:
    using Feature<name>::LOG;

public:
    PulseFeature(Device* device) :
            Feature<name>(device),
            damper() {
        pinMode(gpio, OUTPUT);
        digitalWrite(gpio, invert);
        this->timeoutTimer.initializeMs(100, TimerDelegate(&PulseFeature::onTimeout, this));
        this->registerSubscription("pulse", Device::MessageCallback(&PulseFeature::onMessageReceived, this));
    }

    void doPulse(const uint16_t& duration) {
        digitalWrite(gpio, !invert);
        timeoutTimer.setIntervalMs(min(max_duration, duration));
        timeoutTimer.start(false);
    }

protected:
    virtual void publishCurrentState() {};

private:
    void onMessageReceived(const String& topic, const String& message) {
        if(damper.isDamped()) {
            return;
        }

        const int value = message.toInt();
        if (value <= 0) {
            LOG.log("Message parsed as 0 :", message);
        } else if (value > max_duration) {
            LOG.log("Message > max duration :", message);
        } else {
            this->publish("triggered", String(value), false);
            doPulse(value);
        }
    }

    void onTimeout() {
        digitalWrite(gpio, invert);
    }

    Timer timeoutTimer;
    Damper<damper_time> damper;
};

#endif
