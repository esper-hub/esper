#ifndef ONOFFTEST_H
#define ONOFFTEST_H

#define ON "1"
#define OFF "0"

#include "Feature.h"

template<const char *TYPE_NAME, bool invert = false>
class OnOffFeature : public Feature<TYPE_NAME> {

    const int gpio;
    bool current_state;

public:

    OnOffFeature(HassDevice &device, const char *name, const uint16_t gpio_pin, bool default_state = false) :
            Feature<TYPE_NAME>(device, name),
            gpio(gpio_pin),
            current_state(default_state) {
        this->log("initialized.");
        setState(current_state);
    }

    virtual void onMessageReceived(const String topic, const String message) {
        this->log("topic: ", topic);
        this->log("message: ", message);


        const String token = this->name + "/";
        const auto f = topic.indexOf(token);
        const auto t = topic.substring(f + token.length());

        if (t == "set") {
            handleSetMessage(message);
        } else {
            this->log("unknown message topic received:");
            this->log(t);
        }
    }

protected:
    virtual void registerSubscriptions() {
        this->registerSubscription("set",
                                   MqttStringSubscriptionCallback(&OnOffFeature::onMessageReceived, this));
    }

    inline void handleSetMessage(const String &message) {
        if (message == ON) {
            setState(true);
        } else if (message == OFF) {
            setState(false);
        } else {
            this->log("unknown message received:");
            this->log(message);
        }
    }


    inline void publishCurrentState() {
        this->log("publish.");
        this->publish("state", current_state ? ON : OFF);
    }

    inline void setState(bool desired_state) {
        const bool new_state = invert ? !desired_state : desired_state;
        this->logf("Setting new state: %i %x", gpio, new_state);
        pinMode(gpio, OUTPUT);
        digitalWrite(gpio, new_state);
        current_state = desired_state;

        this->publishCurrentState();
    }
};

#endif
