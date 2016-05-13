#ifndef CYBERHOEHLE_SOCKET_H
#define CYBERHOEHLE_SOCKET_H

#include "OnOffFeature.h"

extern const char _SOCKET[];

class Socket : public OnOffFeature<_SOCKET> {
    using OnOffFeature::OnOffFeature;
};

#endif //CYBERHOEHLE_SOCKET_H
