#ifndef BUILTINLED_H
#define BUILTINLED_H

#include "Logger.h"
#include "Light.h"


template <const char* name>
class BuiltinLED : public Light<name, 2> {
public:
    BuiltinLED(HassDevice &device) :
            Light<name>(device) {
    }
};


#endif
