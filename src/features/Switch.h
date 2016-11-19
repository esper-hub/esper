#ifndef SWITCH_H
#define SWITCH_H

#include "Feature.h"


template<const char* name, uint16_t gpio>
class Switch : public Feature<name> {
    constexpr static const char ON[] = "1";
    constexpr static const char OFF[] = "0";

public:
    Switch(HassDevice &device) :
            Feature<name>(device) {
        attachInterrupt(gpio, Delegate<void()>(&Switch::onInterrupt, this), CHANGE);
    }

    void publishCurrentState() {
        this->logf("Current state: %b", this->state);
        this->publish("state", this->state ? ON : OFF);
    }

private:
    virtual void onInterrupt()  {
        const bool state = digitalRead(gpio);
        if (this->state != state) {
            this->state = state;
            publishCurrentState();
        }
    }

    bool state;
};


#endif
