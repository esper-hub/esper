#ifndef UPDATER_H
#define UPDATER_H

#ifndef UPDATER_URL
#error "UPDATER_URL not defined"
#endif

#ifndef UPDATER_INTERVAL
#error "UPDATER_INTERVAL not defined"
#endif

#ifndef UPDATER_TOPIC
#error "UPDATER_TOPIC not defined"
#endif

#include "Service.h"


extern const char UPDATER_NAME[];

class Updater : public Service<UPDATER_NAME> {
public:
    Updater(Device* const device);
    virtual ~Updater();

private:
    void onMessageReceived(const String& topic, const String& message);

    Timer timer;
};

#endif
