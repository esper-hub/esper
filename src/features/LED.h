#ifndef LED_H
#define LED_H

#include "OnOffFeature.h"


template <const char* name, uint16_t gpio>
class LED : public OnOffFeature<name, gpio, true> {
public:
    LED(HassDevice &device) :
            OnOffFeature<name>(device) {
    }
};


#endif //CYBERHOEHLE_LED_H
