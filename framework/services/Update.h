#ifndef UPDATER_H
#define UPDATER_H

#include <SmingCore/SmingCore.h>
#include <user_config.h>

#include "Service.h"


constexpr const char UPDATE_NAME[] = "update";


class Update : public Service<UPDATE_NAME> {
public:
    Update(Device* device);
    virtual ~Update();

    void checkUpdate();

protected:
    virtual void onConnected();
    virtual void onDisconnected();

private:
    void onUpdateRequestReceived(const String& topic, const String& message);
    void onVersionReceived(HttpClient& client, bool successful);

    Timer timer;

    HttpClient http;

    rBootHttpUpdate* updater;
};

#endif
