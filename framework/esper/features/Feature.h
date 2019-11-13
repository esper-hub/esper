#ifndef FEATURE_H
#define FEATURE_H

#include "../util/Logger.h"
#include "../Device.h"


template<const char* const name>
class Feature : public ServiceBase {
protected:
    static const Logger LOG;

    Feature(Device* const device) :
            device(device) {
    }

    virtual ~Feature() {
    }

public:
    virtual const char* getName() const {
        return name;
    }

    virtual void onStateChanged(const State& state) {
        if (state == State::CONNECTED) {
            this->publishCurrentState();
        }
    }

protected:
    void publish(const String &topic,
                 const String &message,
                 const bool& retain = false) const {
        this->device->publish(buildTopic(topic),
                              message,
                              retain);
    }

    void registerSubscription(const String& topic,
                              const Device::MessageCallback& callback) {
        this->device->registerSubscription(buildTopic(topic),
                                           callback);
    }

    virtual void publishCurrentState() = 0;

private:
    Device* const device;

    static String buildTopic(const String& suffix) {
        StringSumHelper s = Device::TOPIC_BASE;
        s += "/";
        s += name;
        if (suffix.length() != 0) {
            s += "/";
            s += suffix;
        }

        return s;
    }
};


template<const char* const name>
const Logger Feature<name>::LOG = Logger(name);

#endif
