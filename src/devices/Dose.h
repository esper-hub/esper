#ifndef ESPDOSE_H_
#define ESPDOSE_H_

#include "../features/Socket.h"
#include "HassDevice.h"

class Dose : public HassDevice {
    constexpr static const char PLUG1_NAME[] = "sw1";
    constexpr static const uint16_t PLUG1_GPIO = 12;

    constexpr static const char PLUG2_NAME[] = "sw2";
    constexpr static const uint16_t PLUG2_GPIO = 14;

public:
    Dose();

private:
    Socket<PLUG1_NAME, PLUG1_GPIO> socket1;
    Socket<PLUG2_NAME, PLUG1_GPIO> socket2;
};

#endif
