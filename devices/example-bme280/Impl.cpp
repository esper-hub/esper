#include "../features/BME280.h"
#include "Device.h"


constexpr const char BME280_NAME[] = "bme280";
const uint8_t BME280_I2C_ADDR = 0x76;

class Bme280Device : public Device {
public:
    Bme280Device() :
            bme280(this)
    {
        this->add(&(this->bme280));
    }

private:
    BME280<BME280_NAME, BME280_I2C_ADDR> bme280;
};


Device* createDevice() {
    return new Bme280Device();
}


