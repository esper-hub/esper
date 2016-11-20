#include "Dose.h"


constexpr const char Dose::PLUG1_NAME[];
constexpr const uint16_t Dose::PLUG1_GPIO;

constexpr const char Dose::PLUG2_NAME[];
constexpr const uint16_t Dose::PLUG2_GPIO;


Dose::Dose() :
        socket1(this),
        socket2(this) {
    this->add(&(this->socket1));
    this->add(&(this->socket2));
}


Device* createDevice() {
    return new Dose();
}
