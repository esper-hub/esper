#ifndef UPDATER_H
#define UPDATER_H

#ifndef UPDATE_TOPIC
#error "UPDATE_TOPIC not defined"
#endif
#ifndef UPDATE_INTERVAL
#define UPDATE_INTERVAL 3600
#endif

#include "Service.h"


extern const char UPDATE_NAME[];


class Update : public Service<UPDATE_NAME> {
public:
    struct Target {
        RbootOutputStream* stream;
        uint8_t slot;
    };

    Update(Device* device);
    virtual ~Update();

    virtual void onStateChanged(const State& state);

private:
    void check();
    void download();
    void apply(uint8_t slot);

    void onGlobalUpdateRequestReceived(const String& message);
    void onDeviceUpdateRequestReceived(const String& message);

    static Target target();

    Timer checkTimer;

#if UPDATE_DELAY != 0
    Timer delayTimer;
#endif

    Device* device;

    HttpClient http;
};

#endif
