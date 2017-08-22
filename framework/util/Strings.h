#ifndef STRINGS_H
#define STRINGS_H

#include <SmingCore/SmingCore.h>

namespace Strings {
    inline String formatMAC(const uint8_t mac[6]) {
        char buffer[2 * 6 + 1];
        sprintf(buffer,
                "%02x%02x%02x%02x%02x%02x",
                mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

        return String(buffer);
    }
}

#endif
