#ifndef PWM_H
#define PWM_H

#include "Feature.h"
#include "../util/Damper.h"

uint8_t adapt_linear(const uint8_t value) {
    return value;
}

template<const char* const name, uint16_t gpio, uint8_t adapt(const uint8_t) = adapt_linear>
class PWM : public Feature<name>, private DriverPWM {

protected:
    using Feature<name>::LOG;

public:
    PWM(Device* device) :
            Feature<name>(device) {
        this->initialize();
        pinMode(gpio, OUTPUT);
        this->analogWrite(gpio, 0);
        this->registerSubscription("set", Device::MessageCallback(&PWM::onMessageReceived, this));
    }

    void setValue(const uint8_t& value) {
        this->analogWrite(gpio, value);

        this->publish("", String(value), false);
    }

protected:
    virtual void publishCurrentState() {};

private:
    void onMessageReceived(const String& topic, const String& message) {
        const uint8_t value = message.toInt();

        setValue(value);
    }
};

#endif
