#ifndef PULSE_H
#define PULSE_H

#include "Feature.h"
#include "../util/Damper.h"

template<const char* const name, uint16_t gpio, bool invert = false, uint16_t damper_time = 0, uint16_t max_duration = 0>
class Pulse : public Feature<name> {

protected:
    using Feature<name>::LOG;

public:
    Pulse(Device* device) :
            Feature<name>(device),
            damper() {
        pinMode(gpio, OUTPUT);
        digitalWrite(gpio, invert);

        this->timeoutTimer.initializeMs(100, TimerDelegate(&Pulse::onTimeout, this));
        this->registerSubscription(F("pulse"), Device::MessageCallback(&Pulse::onMessageReceived, this));
    }

    void doPulse(const uint16_t& duration) {
        digitalWrite(gpio, !invert);

        this->timeoutTimer.setIntervalMs(min(max_duration, duration));
        this->timeoutTimer.start(false);

        this->publish(F("triggered"), String(duration), false);
    }

protected:
    virtual void publishCurrentState() {};

private:
    void onMessageReceived(const String& topic, const String& message) {
        if(this->damper.isDamped()) {
            return;
        }

        const int duration = message.toInt();
        if (duration <= 0) {
            return;
        }

        doPulse(duration);
    }

    void onTimeout() {
        digitalWrite(gpio, invert);
    }

    Timer timeoutTimer;
    Damper<damper_time> damper;
};

#endif
