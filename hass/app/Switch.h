//
// Created by andi on 5/6/16.
//

#ifndef CYBERHOEHLE_SWITCH_H
#define CYBERHOEHLE_SWITCH_H

#include "Feature.h"

extern const char _SWITCH[];

class Switch : public Feature<_SWITCH> {
    uint8_t gpio_pin;
    bool current_value;
protected:
    inline bool readPin() {
        return digitalRead(gpio_pin);
    }

    void restartTimer();

public:
    Switch(HassDevice &device, const char *name, uint8_t gpio_pin);

    virtual void onStateChanged();

    void publishCurrentState();
};


#endif //CYBERHOEHLE_SWITCH_H
