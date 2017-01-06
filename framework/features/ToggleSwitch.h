#ifndef TOGGLESWITCH_H
#define TOGGLESWITCH_H

#include <SmingCore/SmingCore.h>
#include "Button.h"
#include "Logger.h"


template<const char* name, uint16_t gpio>
class ToggleSwitch : public Button<name, gpio> {
public:
    ToggleSwitch(Device* device, bool initial) :
            Button<name>(device),
            state(initial) {
    }

    void onTimerExpired()  {
        bool value = readPin();
        if (value != pin_value) {
            pin_value = value;
            state = !state;
            publishCurrentState();
        } else {
            restartTimer();
        }
    }

    void onStateChanged()  {
        if (preller.isStarted()) return;
        pin_value = readPin();
        restartTimer();
    }

    void restartTimer()  {
        if (preller.isStarted()) return;
        preller.initializeMs(10, TimerDelegate(&ToggleSwitch::onTimerExpired, this));
    }

private:
    bool state;
    Timer preller;
    bool pin_value;
};


#endif
