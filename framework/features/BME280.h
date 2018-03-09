#ifndef BME280_SENSOR_H
#define BME280_SENSOR_H

#include "Feature.h"
#include <Libraries/Adafruit_BME280_Library/Adafruit_BME280.h>

#define SEALEVELPRESSURE_HPA (1013.25)

template<const char* const name, uint8_t addr>
class BME280 : public Feature<name> {

protected:
    using Feature<name>::LOG;
    Adafruit_BME280 bme280;

public:

    BME280(Device* device) :
        Feature<name>(device) {

        if (!bme280.begin(addr)) {
            Serial.println("Could not detect a BME280 sensor, check wiring!");
            abort();
        }

        delay(100); // Wait for the sensor to boot up

        this->updateTimer.initializeMs(15000, TimerDelegate(&BME280::publishCurrentState, this));
        this->updateTimer.start(true);
    }

protected:
    virtual void publishCurrentState() {
        const long pressure = bme280.readPressure();
        LOG.log("Pressure:", pressure);
        this->publish("pressure", String(pressure));

        const float temperature = bme280.readTemperature();
        LOG.log("Temperature:", temperature);
        this->publish("temperature", String(temperature));

        const float humidity = bme280.readHumidity();
        LOG.log("Humidity", humidity);
        this->publish("humidity", String(humidity));
    }

private:
    Timer updateTimer;

};


#endif
