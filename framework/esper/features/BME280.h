#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include "Feature.h"
#include <Libraries/Adafruit_BME280_Library/Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

template<const char* const name, uint8_t addr, uint8_t sda_pin, uint8_t scl_pin>
class BME280 : public Feature<name> {

protected:
    using Feature<name>::LOG;

public:
    BME280(Device* device) :
        Feature<name>(device) {

        this->wire.begin(sda_pin, scl_pin);
        if (!this->bme280.begin(addr, &this->wire)) {
            LOG.log(F("Could not detect a BME280 sensor, check wiring:"), addr);
            abort();
        }

        delay(100); // Wait for the sensor to boot up

        this->updateTimer.initializeMs(15000, std::bind(&BME280::publishCurrentState, this));
        this->updateTimer.start(true);
    }

protected:
    virtual void publishCurrentState() {
        const long pressure = bme280.readPressure();
        LOG.log(F("Pressure:"), pressure);
        this->publish(F("pressure"), String(pressure));

        const float temperature = bme280.readTemperature();
        LOG.log(F("Temperature:"), temperature);
        this->publish(F("temperature"), String(temperature));

        const float humidity = bme280.readHumidity();
        LOG.log(F("Humidity"), humidity);
        this->publish(F("humidity"), String(humidity));
    }

private:
    TwoWire wire;
    Adafruit_BME280 bme280;

    Timer updateTimer;
};


#endif
