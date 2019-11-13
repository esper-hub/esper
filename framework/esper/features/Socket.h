#ifndef SOCKET_H
#define SOCKET_H

#include "OnOffFeature.h"


template <const char* const name, uint16_t gpio, bool invert=false>
class Socket : public OnOffFeature<name, gpio, invert, 1000> {
    using OnOffFeature<name, gpio, invert, 1000>::OnOffFeature;
};

#endif
