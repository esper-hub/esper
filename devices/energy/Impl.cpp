#include "Device.h"
#include "../features/Feature.h"


template<const char* const name, uint16_t gpio, uint16_t damper = 0>
class PulseFeature : public Feature<name> {
    using Feature<name>::LOG;

public:
    PulseFeature(Device* const device) :
            Feature<name>(device),
            counter(0),
            lastChange(RTC.getRtcSeconds()) {
        pinMode(gpio, INPUT);
        pullup(gpio);
        attachInterrupt(gpio,
                        Delegate<void()>(&PulseFeature::onInterrupt, this),
                        GPIO_INT_TYPE::GPIO_PIN_INTR_POSEDGE);

        LOG.log("Initialized");
    }

    ~PulseFeature() {
    }

protected:
    virtual void publishCurrentState() {
        this->publish("count", String(this->counter));
    }

    virtual void registerSubscriptions() {
    }

    void onInterrupt() {
        const uint32_t now = RTC.getRtcSeconds();

        if (damper > 0) {
            if (this->lastChange + damper > now)
                return;
        }

        this->counter++;
        this->publishCurrentState();

        LOG.log("Counter: ", this->counter);

        this->lastChange = now;
    }

private:
    uint32_t counter;

    uint32_t lastChange;
};



constexpr const char PULSE_NAME[] = "pulse";
constexpr const uint16_t PULSE_GPIO = 2;


class EnergyDevice : public Device {
public:
    EnergyDevice() :
            pulse(this) {
        this->add(&(this->pulse));
    }

private:
    PulseFeature<PULSE_NAME, PULSE_GPIO, 1> pulse;
};


Device* createDevice() {
    return new EnergyDevice();
}
