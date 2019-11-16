#ifndef LED_H
#define LED_H

#include "OnOffFeature.h"


template<const char* const name, uint16_t gpio>
class Light : public OnOffFeature<name, gpio, true> {
public:
    Light(Device* const device) :
            OnOffFeature<name, gpio, true>(device) {
    }
};


#endif
