#ifndef CHECKSUM_H
#define CHECKSUM_H

#include <SmingCore/SmingCore.h>


/**
 * Implementation of SDBM hash algorithm.
 */
constexpr uint16_t hash(const char* const s) {
    return (*s != '\0') ? *s + hash(s + 1) * 65599 : 5381;
}

/**
 * Implementation of the CRC-16-CCITT algorithm.
 */
using checksum_t = uint16_t;

template<class... Args>
inline checksum_t checksum(const checksum_t& crc, const Args&... args);

template<class Arg1, class Arg2, class... Args>
inline checksum_t checksum(const checksum_t& crc, const Arg1& arg1, const Arg2& arg2, const Args&... args) {
    return checksum(checksum(crc, arg1), arg2, args...);
}

template<>
inline checksum_t checksum<>(const checksum_t& crc) {
    return crc;
}

template<>
inline checksum_t checksum<>(const checksum_t& crc, const uint8_t& val) {
    uint8_t x = crc >> 8;
    x ^= val;
    x ^= x >> 4;

    checksum_t res = (crc << 8);
    res ^= (x << 12);
    res ^= (x << 5);
    res ^= (x << 0);

    return res;
}

template<>
inline checksum_t checksum<>(const checksum_t& crc, const uint16_t& val) {
    return checksum(crc,
            (uint8_t) (val >> 8),
            (uint8_t) (val >> 0));
}

template<>
inline checksum_t checksum<>(const checksum_t& crc, const uint32_t& val) {
    return checksum(crc,
            (uint8_t) (val >> 24),
            (uint8_t) (val >> 16),
            (uint8_t) (val >> 8),
            (uint8_t) (val >> 0));
}

#endif
