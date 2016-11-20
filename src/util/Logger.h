#ifndef LOGGER_H
#define LOGGER_H

#include <SmingCore/SmingCore.h>

class Logger {
public:
    Logger(const char* const prefix) :
            prefix(prefix) {
    }

    template<typename... Args>
    inline void log(const Args&... args) const {
        this->printPrefix();
        this->printArgs(args...);
    }

private:
    inline void printPrefix() const {
        Serial.print('[');
        Serial.print(prefix);
        Serial.print(']');
        Serial.print(' ');
    };

    template<typename Head>
    inline void printArgs(const Head& head) const {
        Serial.print(head);
        Serial.println();
    }

    template<typename Head, typename... Tail>
    inline void printArgs(const Head& head, const Tail&... tail) const {
        Serial.print(head);
        Serial.print(' ');

        printArgs<Tail...>(tail...);
    };

    const char* const prefix;
};

#endif
