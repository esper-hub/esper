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
    Update(Device* device);
    virtual ~Update();

    virtual void onStateChanged(const State& state);

    void checkUpdate();

private:
    void onGlobalUpdateRequestReceived(const String& message);
    void onDeviceUpdateRequestReceived(const String& message);

    int onVersionReceived(HttpConnection& client, bool successful);

    Timer checkTimer;

#if UPDATE_DELAY != 0
    Timer delayTimer;
#endif

    HttpClient http;

    RbootHttpUpdater* updater;
};

#endif
