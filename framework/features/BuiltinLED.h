#ifndef BUILTINLED_H
#define BUILTINLED_H

#include "Light.h"


template <const char* const name>
class BuiltinLED : public Light<name, 2> {
public:
    BuiltinLED(Device* const device) :
            Light<name, 2>(device) {
    }
};


#endif
