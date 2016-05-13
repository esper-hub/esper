//
// Created by andi on 5/13/16.
//

#ifndef CYBERHOEHLE_LED_H
#define CYBERHOEHLE_LED_H

#include "OnOffFeature.h"

extern const char _LED[];

class LED : public OnOffFeature<_LED, true> {
public:
    LED(HassDevice &device, const char *name, const uint16_t gpio_pin);
};


#endif //CYBERHOEHLE_LED_H
