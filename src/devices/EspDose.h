#ifndef ESPDOSE_H_
#define ESPFOSE_H_

#include "../features/Socket.h"
#include "Device.h"

class EspDose : public Device {
    enum {
        PLUG1_GPIO = 12,
        PLUG2_GPIO = 14
    };
public:
    EspDose() : Device("EspDose") {
        getDevice()->add<Socket>("sw1", PLUG1_GPIO);
        getDevice()->add<Socket>("sw2", PLUG2_GPIO);
    }
};

#endif
