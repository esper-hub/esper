#ifndef BUILTINLED_H
#define BUILTINLED_H

#include "Logger.h"
#include "LED.h"


template <const char* name>
class BuiltinLED : public LED<name, 2> {
public:
    BuiltinLED(HassDevice &device) :
            LED<name>(device) {
    }
};


#endif
