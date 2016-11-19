#ifndef HASS_DEVICE_H
#define HASS_DEVICE_H

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

    const String basePath;

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;

    Vector<FeatureBase*> features;

    HashMap<String, MessageCallback> messageCallbacks;
};

#endif
