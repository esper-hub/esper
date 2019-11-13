#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <SmingCore.h>


/**
 * Implementation of SDBM hash algorithm.
 */
constexpr uint16_t hash(const char* const s) {
    return (*s != '\0') ? *s + hash(s + 1) * 65599 : 5381;
}

/**
 * Implementation of the CRC-16-CCITT algorithm.
 */
class Checksum {
public:
    using value_t = uint16_t;

    Checksum(const value_t& value) :
            value(value) {
    }

    Checksum(const Checksum& checksum) :
            value(checksum.value) {
    }

    Checksum operator<<(const uint8_t val) {
        uint8_t x = this->value >> 8;
        x ^= val;
        x ^= x >> 4;

        value_t value = (this->value << 8);
        value ^= (x << 12);
        value ^= (x << 5);
        value ^= (x << 0);

        return Checksum(value);
    }

    operator const value_t&() const {
        return this->value;
    }

private:
    const value_t value;
};

Checksum operator<<(Checksum checksum, const uint16_t& val) {
    return checksum
           << (uint8_t)(val >> 8)
           << (uint8_t)(val >> 0);
}

Checksum operator<<(Checksum checksum, const uint32_t& val) {
    return checksum
           << (uint8_t)(val >> 24)
           << (uint8_t)(val >> 26)
           << (uint8_t)(val >> 8)
           << (uint8_t)(val >> 0);
}

Checksum operator<<(Checksum checksum, const uint64_t& val) {
    return checksum
           << (uint8_t)(val >> 56)
           << (uint8_t)(val >> 48)
           << (uint8_t)(val >> 40)
           << (uint8_t)(val >> 32)
           << (uint8_t)(val >> 24)
           << (uint8_t)(val >> 26)
           << (uint8_t)(val >> 8)
           << (uint8_t)(val >> 0);
}

#endif
