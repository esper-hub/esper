#ifndef DEVICE_H
#define DEVICE_H

#include <SmingCore.h>

#include "util/Logger.h"
#include "managers/WifiConnectionManager.h"
#include "managers/MqttConnectionManager.h"

#include "services/Info.h"
#include "services/Reboot.h"

#if HEARTBEAT_ENABLED
#include "services/Heartbeat.h"
#endif

#if UPDATE_ENABLED
#include "services/Update.h"
#endif


class Device {
    static const Logger LOG;

    Device(Device const&) = delete;
    Device& operator=(Device const&) = delete;

public:
    using MessageCallback = MqttConnectionManager::MessageCallback;

    static const String TOPIC_BASE;

    Device();
    virtual ~Device();

    virtual void start();

    void triggerReboot();

    void registerSubscription(const String& topic, const MessageCallback& callback);

    void publish(const String &topic, const String &message, const bool& retain = false);

    void add(ServiceBase* const service);

    const WifiConnectionManager& getWifi() const;
    const MqttConnectionManager& getMqtt() const;

    const Vector<ServiceBase*>& getServices() const;
    const HashMap<String, MessageCallback>& getSubscriptions() const;

private:
    void onWifiStateChanged(const WifiConnectionManager::State& state);

    void onMqttStateChanged(const MqttConnectionManager::State& state);

    void onMqttMessageReceived(const String& topic, const String& message);

    void onTimeUpdated(NtpClient& client, time_t time);

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;

    NtpClient ntpClient;

    Vector<ServiceBase*> services;

    HashMap<String, MessageCallback> messageCallbacks;

#if HEARTBEAT_ENABLED
    Heartbeat heartbeat;
#else
    #warning "Heartbeat disabled" 
#endif

#if UPDATE_ENABLED
    Update update;
#else
    #warning "Update disabled" 
#endif

    Info info;
    Reboot reboot;
};


Device* createDevice();

#endif
