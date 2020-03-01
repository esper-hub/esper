#ifndef LOGGER_H
#define LOGGER_H

#include <SmingCore.h>


template<HardwareSerial* const serial>
class LoggerImpl {
public:
    LoggerImpl(const char* const prefix) :
            prefix(prefix) {
    }

    template<typename... Args>
    inline void log(const Args& ... args) const {
        this->printPrefix();
        this->printArgs(args...);
    }

    static void init() {
        serial->begin(SERIAL_BAUD_RATE); // Start serial
        serial->systemDebugOutput(LOGGING_DEBUG); // Debug output to serial
        serial->commandProcessing(false);
    }

private:
    inline void printPrefix() const {
        serial->print(SystemClock.getSystemTimeString());
        serial->print(' ');
        serial->print('[');
        serial->print(prefix);
        serial->print(']');
        serial->print(' ');
    };

    template<typename Head>
    inline void printArgs(const Head& head) const {
        serial->print(head);
        serial->print('\n');
    }

    template<typename Head, typename... Tail>
    inline void printArgs(const Head& head, const Tail& ... tail) const {
        serial->print(head);
        serial->print(' ');

        printArgs<Tail...>(tail...);
    };

    const char* const prefix;
};

template<>
class LoggerImpl<nullptr> {
public:
    LoggerImpl(const char* const prefix) {
    }

    template<typename... Args>
    inline void log(const Args& ... args) const {
    }

    static void init() {
    }
};


#if LOGGING
#if !LOGGING_ALT
typedef LoggerImpl<&Serial> Logger;
#else
extern HardwareSerial Serial1;
typedef LoggerImpl<&Serial1> Logger;
#endif
#else
typedef LoggerImpl<nullptr> Logger;
#endif

#endif
