#ifndef FEATURE_H
#define FEATURE_H

#include "HassDevice.h"
#include "Log.h"

class BaseFeature {
protected:
	virtual void publish(const String& partial_topic, const String& message) const  = 0;
	//{
	//	const String topic(REALM "/" + partial_topic);
	//	
	//}
};

template<const char* TYPE_NAME>
class Feature : public BaseFeature, public Log<TYPE_NAME> {
protected:
    HassDevice& device;
    String base_topic;

public:

    Feature(HassDevice& device, const char* name) : device(device) {
        this->base_topic += TYPE_NAME;
        this->base_topic += "/";
        this->base_topic += name;
    }

    virtual void publish(const String& partial_topic, const String& message) const {
	    device.publish(partial_topic, message);
    }

    virtual void onMessageReceived(const String& topic, const String& message) = 0;
    virtual void onMqttConnected() {
        publishCurrentState();
    }
    virtual void publishCurrentState() = 0;
    inline const String& getBaseTopic() const {
        return base_topic;
    }
};

#endif
