#ifndef ONOFFTEST_H
#define ONOFFTEST_H

#define ON "1"
#define OFF "0"


template<const char* TYPE_NAME, bool invert = false>
class OnOffFeature : public Feature<TYPE_NAME> {

    const int gpio;
    bool current_state;

public:

    OnOffFeature(HassDevice& device, const char* name, const int gpio_pin, bool default_state=false) :
	    Feature<TYPE_NAME>(device, name),
	    gpio(gpio_pin),
	    current_state(default_state) {
        setState(current_state);
    }

    virtual void onMessageReceived(const String& topic, const String& message) {
        this->log("topic: ", topic);
	this->log("message: ", message);
        if (topic == "set") {
            handleSetMessage(message);
        } else {
            this->log("unknown message topic received:");
            this->log(topic);
        }
    }

    inline void handleSetMessage(const String& message) {
        if (message == ON) {
            setState(true);
        } else if (message == OFF) {
            setState(false);
        } else {
            this->log("unknown message received:");
            this->log(message);
        }
    }

protected:

    inline void publishCurrentState() {
        this->publish("state", current_state ? ON : OFF);
    }

    inline void setState(bool desired_state) {
	const bool new_state = invert ? !desired_state : desired_state;
	digitalWrite(gpio, new_state);
        current_state = desired_state;

        this->publishCurrentState();
    }
};

#endif
