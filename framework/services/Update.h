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

    void checkUpdate();

private:
    void onUpdateRequestReceived(const String& topic, const String& message);
    void onVersionReceived(HttpClient& client, bool successful);

    Timer timer;

    HttpClient http;

    rBootHttpUpdate* updater;
};

#endif
