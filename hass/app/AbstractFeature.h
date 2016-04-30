#ifndef ABSTRACTFEATURE_H
#define ABSTRACTFEATURE_H


class AbstractFeature {
protected:
	virtual void publish(const String& partial_topic, const String& message) const  = 0;
    virtual void publishCurrentState(void) = 0;
    virtual void onMessageReceived(const String topic, const String message) = 0;
    virtual void registerSubscriptions(void) = 0;
public:
    virtual void onMqttConnected() = 0;
};

#endif //ABSTRACTFEATURE_H
