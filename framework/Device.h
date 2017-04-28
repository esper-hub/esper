#ifndef DEVICE_H
#define DEVICE_H

#include <SmingCore/SmingCore.h>

#include "util/Logger.h"
#include "managers/WifiConnectionManager.h"
#include "managers/MqttConnectionManager.h"

#include "services/Info.h"
#include "services/Heartbeat.h"
#include "services/Update.h"


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

#if HEARTBEAT_ENABLED
    Heartbeat heartbeat;
#endif

#if UPDATE_ENABLED
    Update update;
#endif

    Info info;
};


Device* createDevice();

#endif
