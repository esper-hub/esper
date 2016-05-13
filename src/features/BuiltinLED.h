#ifndef CYBERHOEHLE_BUILTINLED_H
#define CYBERHOEHLE_BUILTINLED_H

#include "../util/Log.h"
#include "LED.h"

extern const char _BUILTIN_LED[];

class BuiltinLED : public LED, public Log<_BUILTIN_LED> {

    enum {
        LED_GPIO = 2
    };

public:
    BuiltinLED(HassDevice &device, const char *name);
};


#endif //CYBERHOEHLE_BUILTINLED_H
