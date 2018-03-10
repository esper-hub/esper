#ifndef DS1820SENSOR_H
#define DS1820SENSOR_H

#include "Feature.h"

#include <Libraries/OneWire/OneWire.h>
#include <Libraries/DS18S20/ds18s20.h>


template<const char* const name, int gpio>
class DS18S20Sensor : public Feature<name> {

protected:
    DS18S20 ds18s20;

public:
    DS18S20Sensor(Device* device) : Feature<name>(device) {
        this->ds18s20.Init(gpio);
	this->ds18s20.RegisterEndCallback(DS18S20CompletedDelegate(&DS18S20Sensor::measureCompleted, this));

        for (int i = 0; i < this->ds18s20.GetSensorsCount(); i++) {
            const uint64_t id = this->ds18s20.GetSensorID(i);
            sprintf(this->topics[i], "%08X-%08X/temperature", (uint32_t)(id>>32), (uint32_t)(id>>0));
        }

        this->updateTimer.initializeMs(15000, TimerDelegate(&DS18S20::StartMeasure, &this->ds18s20));
        this->updateTimer.start(true);
    }

protected:
    void publishCurrentState() {
    }

private:
    void measureCompleted() {
        for(int i = 0; i < this->ds18s20.GetSensorsCount(); i++) {
            if (this->ds18s20.IsValidTemperature(i)) {
                const float temperature = this->ds18s20.GetCelsius(i);
                this->publish(this->topics[i], String(temperature), true);
            }
        }
    }

    char topics[MAX_SENSORS][42];

    Timer updateTimer;
};


#endif
