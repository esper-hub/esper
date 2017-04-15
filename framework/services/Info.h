#ifndef INFO_H
#define INFO_H

#include "Service.h"

extern const char INFO_NAME[];

class Info : public Service<INFO_NAME> {
public:
    Info(Device* const device);
    virtual ~Info();

    virtual void onStateChanged(const State& state);

private:
    void publish();

    Timer timer;

    uint32_t startupTime;
    uint32_t connectTime;
};


#endif
