#ifndef CYBERHOEHLE_TOGGLESWITCH_H
#define CYBERHOEHLE_TOGGLESWITCH_H

#include <SmingCore/SmingCore.h>
#include "Switch.h"
#include "Log.h"

extern const char _TOGGLE_SWITCH[];

class ToggleSwitch : public Switch, public Log<_TOGGLE_SWITCH> {
    bool state;
    Timer preller;
    bool pin_value;
public:
    ToggleSwitch(HassDevice &device, const char *name, uint8_t gpio_pin, bool initial);

    void onTimerExpired();

    void onStateChanged();

    void restartTimer();
};


#endif //CYBERHOEHLE_TOGGLESWITCH_H
