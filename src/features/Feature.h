#ifndef FEATURE_H
#define FEATURE_H

#include "Logger.h"
#include "../devices/HassDevice.h"


template<const char* name>
class Feature : public FeatureBase {
protected:
    static const Logger LOG;

public:
    using MessageCallback = HassDevice::MessageCallback;

protected:
    Feature(HassDevice* device) :
            device(device) {
    }

public:
    virtual const char* getName() const {
        return name;
    }

protected:
    void publish(const String &topic, const String &message) const {
        this->device->publish(name + '/' + topic, message);
    }

    virtual void registerSubscription(const String& topic, MessageCallback callback) {
        this->device->registerSubscription(this->getName() + '/' + topic, callback);
    }

    virtual void onMessageReceived(const String& topic, const String& message) = 0;

private:
    HassDevice* const device;
};


template<const char* name>
const Logger Feature<name>::LOG = Logger(name);

#endif
