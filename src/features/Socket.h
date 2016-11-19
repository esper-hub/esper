#ifndef SOCKET_H
#define SOCKET_H

#include "OnOffFeature.h"


template <const char* name, uint16_t gpio>
class Socket : public OnOffFeature<name, gpio, false, 1> {
    using OnOffFeature<name, gpio, false, 1>::OnOffFeature;
};

#endif
