#ifndef FEATURE_H
#define FEATURE_H

#include "Logger.h"
#include "Device.h"


template<const char* name>
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
        this->device->publish(name + '/' + topic, message);
    }

    virtual void registerSubscription(const String& topic, const Device::MessageCallback& callback) {
        this->device->registerSubscription(this->getName() + '/' + topic, callback);
    }

    virtual void onMessageReceived(const String& topic, const String& message) = 0;

private:
    Device* const device;
};


template<const char* name>
const Logger Feature<name>::LOG = Logger(name);

#endif
