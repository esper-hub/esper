#ifndef REBOOT_H
#define REBOOT_H

#include "Service.h"

extern const char REBOOT_NAME[];

class Reboot : public Service<REBOOT_NAME> {
public:
    Reboot(Device* const device);
    virtual ~Reboot();

    void trigger(unsigned int deferMillis = 0);

private:
    void onRebootRequestReceived(const String& message);
};


#endif
