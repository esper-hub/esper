//
// Created by andi on 5/13/16.
//

#include "BuiltinLED.h"

const char _BUILTIN_LED[] = "[BUILTINLED] ";

BuiltinLED::BuiltinLED(HassDevice &device, const char *name) :
        LED(device, name, LED_GPIO) { }