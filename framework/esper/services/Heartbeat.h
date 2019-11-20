#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#ifndef HEARTBEAT_TOPIC
#error "HEARTBEAT_TOPIC not defined"
#endif

#include "Service.h"


extern const char HEARTBEAT_NAME[];

class Heartbeat : public Service<HEARTBEAT_NAME> {
public:
    Heartbeat(Device* const device);
    virtual ~Heartbeat();

    virtual void onStateChanged(const State& state);

private:
    void onMessageReceived(const String& message);

    Timer timer;
};


#endif
