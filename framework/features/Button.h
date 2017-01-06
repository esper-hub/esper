#ifndef BUTTON_H
#define BUTTON_H

#include "Feature.h"


template<const char* name, uint16_t gpio, bool inverted>
class Button : public Feature<name> {
    using Feature<name>::LOG;

    constexpr static const char* const ON = "1";
    constexpr static const char* const OFF = "0";

public:
    Button(Device* device) :
            Feature<name>(device),
            state(false) {
        attachInterrupt(gpio, Delegate<void()>(&Button::onInterrupt, this), CHANGE);
    }

    void publishCurrentState() {
        LOG.log("Current state:", this->state);

        this->publish("state", this->state ? ON : OFF);
    }

    void registerSubscriptions() {
    }

private:
    virtual void onInterrupt()  {
        const bool state = (digitalRead(gpio) == !inverted);
        if (this->state != state) {
            this->state = state;
            publishCurrentState();
        }
    }

    bool state;
};


#endif
