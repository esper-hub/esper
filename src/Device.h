#ifndef DEVICE_H
#define DEVICE_H

#include <SmingCore/SmingCore.h>
#include <user_config.h>

#include "../util/Logger.h"
#include "../managers/WifiConnectionManager.h"
#include "../managers/MqttConnectionManager.h"


class FeatureBase {
protected:
    explicit FeatureBase();
    virtual ~FeatureBase();

public:
    virtual const char* getName() const = 0;

    virtual void registerSubscriptions() = 0;
    virtual void publishCurrentState() = 0;
};


class Device {
    static const Logger LOG;

    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;

public:
    using MessageCallback = MqttConnectionManager::MessageCallback;

    Device();
    virtual ~Device();

    void start();

    void reboot();

    void registerSubscription(const String& topic, const MessageCallback& callback);

    void add(FeatureBase* feature);

    void publish(const String &topic, const String &message);

private:
    void onWifiStateChanged(const WifiConnectionManager::State& state);

    void onMqttStateChanged(const MqttConnectionManager::State& state);

    void onMqttMessageReceived(const String& topic, const String& message);

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;

    const String topicBase;

    Vector<FeatureBase*> features;

    HashMap<String, MessageCallback> messageCallbacks;
};


Device* createDevice();

#endif
