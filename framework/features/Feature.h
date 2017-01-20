#ifndef FEATURE_H
#define FEATURE_H

#include "Logger.h"
#include "Device.h"


template<const char* const name>
class Feature : public FeatureBase {
protected:
    static const Logger LOG;

    Feature(Device* device) :
            device(device) {
    }

    virtual ~Feature() {
    }

public:
    virtual const char* getName() const {
        return name;
    }

protected:
    void publish(const String &topic, const String &message) const {
        this->device->publish(name + ("/" + topic), message);
    }

    void registerSubscription(const String& topic, const Device::MessageCallback& callback) {
        this->device->registerSubscription(name + ("/" + topic), callback);
    }

private:
    Device* const device;
};


template<const char* const name>
const Logger Feature<name>::LOG = Logger(name);

#endif
