#ifndef LOG_H
#define LOG_H

#include <SmingCore/SmingCore.h>

template<const char *prefix>
class Log {

private:
    inline void _log_prefix() const {
        Serial.print(prefix);
    }

public:
    template<typename T>
    inline void log(const T &msg) const {
        _log_prefix();
        Serial.println(msg);
    }

    template<typename T, typename B>
    inline void log(const T &msgA, const B &msgB) const {
        _log_prefix();
        Serial.print(msgA);
        Serial.println(msgB);
    }

    template<typename StringType,
            typename... Args>
    inline void logf(const StringType &msg, Args... args) const {
        _log_prefix();
        Serial.printf(msg, args...);
    };

};

#endif
