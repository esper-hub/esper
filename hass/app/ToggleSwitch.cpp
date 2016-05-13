//
// Created by andi on 5/6/16.
//

#include "ToggleSwitch.h"


const char _TOGGLE_SWITCH[] = "ToggleSwitch";

ToggleSwitch::ToggleSwitch(HassDevice &device, const char *name, uint8_t gpio_pin, bool initial) :
        Switch(device, name, gpio_pin), state(initial) {
}

void ToggleSwitch::onStateChanged() {
    if (preller.isStarted()) return;
    pin_value = readPin();
    restartTimer();
}

void ToggleSwitch::onTimerExpired() {
    bool value = readPin();
    if (value != pin_value) {
        pin_value = value;
        state = !state;
        publishCurrentState();
    } else {
        restartTimer();
    }
}

void ToggleSwitch::restartTimer() {
    if (preller.isStarted()) return;
    preller.initializeMs(10, TimerDelegate(&ToggleSwitch::onTimerExpired, this));
}