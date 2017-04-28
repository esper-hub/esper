#ifndef DEVICE_H
#define DEVICE_H

#include <SmingCore/SmingCore.h>

#include "util/Logger.h"
#include "managers/WifiConnectionManager.h"
#include "managers/MqttConnectionManager.h"


class ServiceBase {
public:
    enum class State {
        CONNECTED,
        DISCONNECTED
    };

protected:
    explicit ServiceBase();
    virtual ~ServiceBase();

public:
    virtual const char* getName() const = 0;

    virtual void onStateChanged(const State& state) = 0;
};


class Device {
    static const Logger LOG;

    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;

public:
    using MessageCallback = MqttConnectionManager::MessageCallback;

    static const String TOPIC_BASE;

    Device();
    virtual ~Device();

    void start();

    void reboot();

    void registerSubscription(const String& topic, const MessageCallback& callback);

    void publish(const String &topic, const String &message, const bool& retain = false);

protected:
    void add(ServiceBase* const service);

private:
    void onWifiStateChanged(const WifiConnectionManager::State& state);

    void onMqttStateChanged(const MqttConnectionManager::State& state);

    void onMqttMessageReceived(const String& topic, const String& message);

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;

    Vector<ServiceBase*> services;

    HashMap<String, MessageCallback> messageCallbacks;
};


Device* createDevice();

#endif
