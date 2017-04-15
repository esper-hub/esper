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
        this->device->publish(name + ("/" + topic), message, retain);
    }

    void registerSubscription(const String& topic,
                              const Device::MessageCallback& callback) {
        this->device->registerSubscription(name + ("/" + topic), callback);
    }

    virtual void publishCurrentState() = 0;

private:
    Device* const device;
};


template<const char* const name>
const Logger Feature<name>::LOG = Logger(name);

#endif
