#ifndef STATUS_H
#define STATUS_H


#include <SmingCore/SmingCore.h>
#include <user_config.h>

#include "Service.h"


constexpr const char STATUS_NAME[] = "status";


class Status : public Service<STATUS_NAME> {
public:
    Status(Device* device);
    virtual ~Status();

protected:
    virtual void onConnected();
    virtual void onDisconnected();

private:
};


#endif
