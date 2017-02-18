#ifndef PULSE_H
#define PULSE_H

#include "features/Feature.h"
#include "util/Persisted.h"


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
        this->publish("count", String(*this->counter));
    }

    virtual void registerSubscriptions() {
        this->registerSubscription("set", Device::MessageCallback(&PulseFeature::onMessageReceived, this));
    }

    void onMessageReceived(const String& topic, const String& message) {
        this->counter.write(message.toInt());
        LOG.log("Changed counter:", *this->counter);
    }

    void onInterrupt() {
        const uint32_t now = RTC.getRtcSeconds();

        if (damper > 0) {
            if (this->lastChange + damper > now)
                return;
        }

        this->counter.write(*this->counter + 1);
        this->publishCurrentState();

        LOG.log("Counter:", *this->counter);

        this->lastChange = now;
    }

private:
    Persisted<uint32_t> counter;

    uint32_t lastChange;
};


#endif
