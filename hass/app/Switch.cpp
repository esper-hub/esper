#include "Switch.h"
#include <SmingCore/SmingCore.h>

const char _SWITCH[] = "SWITCH";

Switch::Switch(HassDevice &device, const char *name, uint8_t gpio_pin) : Feature(device, name), gpio_pin(gpio_pin) {
    attachInterrupt(gpio_pin, Delegate<void()>(&Switch::onStateChanged, this), CHANGE);
}

void Switch::onStateChanged() {
    const bool state = readPin();
    if (state != current_value) {
        current_value = state;
        publishCurrentState();
    }
}

void Switch::publishCurrentState() {
    this->logf("Current state: %b", current_value);
    this->publish(this->name + "/state/", current_value ? "ON" : "OFF");
}