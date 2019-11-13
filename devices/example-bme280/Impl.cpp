#include <features/BME280.h>

#include "Device.h"


constexpr const char BME280_NAME[] = "bme280";
constexpr const uint8_t BME280_ADDR = 0x76;
constexpr const uint8_t BME280_SDA_PIN = 2;
constexpr const uint8_t BME280_SCL_PIN = 0;

class Bme280Device : public Device {
public:
    Bme280Device() :
            bme280(this) {
        this->add(&(this->bme280));
    }

private:
    BME280<BME280_NAME, BME280_ADDR, BME280_SDA_PIN, BME280_SCL_PIN> bme280;
};


Device* createDevice() {
    return new Bme280Device();
}


