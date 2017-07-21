#ifndef BUILTINLED_H
#define BUILTINLED_H

#include "Light.h"

#define BUILTIN_LED_GPIO 2

template <const char* const name>
class BuiltinLED : public Light<name, BUILTIN_LED_GPIO> {
public:
    BuiltinLED(Device* device) :
            Light<name, BUILTIN_LED_GPIO>(device) {
    }
};


#endif
