#ifndef PULSE_H
#define PULSE_H

#include "features/Feature.h"
#include "util/Persisted.h"


template<const char* const name, uint16_t gpio, uint64_t damper = 0>
class PulseFeature : public Feature<name> {
    using Feature<name>::LOG;

    struct record_t {
        uint64_t time;
        uint32_t count;

        friend Checksum operator<<(Checksum checksum, const record_t& val) {
            return checksum
                   << val.time
                   << val.count;
        }
    };

public:
    PulseFeature(Device* const device) :
            Feature<name>(device),
            persisted({RTC.getRtcSeconds(), 0}) {
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
        this->publish("count", String(this->persisted->count));
    }

    virtual void registerSubscriptions() {
        this->registerSubscription("set", Device::MessageCallback(&PulseFeature::onMessageReceived, this));
    }

    void onMessageReceived(const String& topic, const String& message) {
        this->persisted.write({RTC.getRtcSeconds(),
                               (uint32_t) message.toInt()});
        LOG.log("Changed counter:", this->persisted->count);
    }

    void onInterrupt() {
        const uint64_t now = RTC.getRtcNanoseconds();

        const uint64_t duration = now - this->persisted->time;

        // Skip interrupt if in damping interval
        if (duration < damper * NS_PER_SECOND) {
            return;
        }

        // Calculate updated counter and gauge
        const uint32_t count = this->persisted->count + 1;
        const float gauge = (float) NS_PER_SECOND / (float) duration;

        this->publish("count", String(count));
        this->publish("gauge", String(gauge, 8));

        LOG.log("Count:", count);
        LOG.log("Gauge:", gauge);

        this->persisted.write({now, count});
    }

private:
    Persisted <record_t> persisted;
};


#endif
