#ifndef HASSDEVICE_HPP
#define HASSDEVICE_HPP

#include <SmingCore/SmingCore.h>
#include <user_config.h>
#include "MqttNode.h"
#include "WifiConnectionManager.h"
#include "MqttConnectionManager.h"


extern const char HASS_DEVICE_LOG_NAME[];
class HassDevice : Log<HASS_DEVICE_LOG_NAME>{
    const String verbose_name;
    const String mac;
    const String basePath;

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;
   
    HashMap<String, MqttStringSubscriptionCallback> messageCallbacks;
    Vector<MqttConnectionManager::MqttConnectionStateChangedDelegate> mqttStateChangeCallbacks;

private:
    void connectWifi();

    inline void _log_prefix() const {
        Serial.print("[HassDevice] ");
    }

    void wifiConnectFail();
    void wifiConnectOk();
    void restart();

    void mqttConnect();

    void onWifiStateChanged(WifiConnectionManager::WifiState state);
    void onMqttStateChanged(MqttConnectionManager::MqttState state);

    void onMqttMessageReceived(String topic, String message);
    void onMqttConnected();
  
    void registerSubscription(const String& topic, MqttStringSubscriptionCallback& cb);
    void mqttSubscribe(const String& topic); 
protected:
public:


    HassDevice(const String verbose_name);
    void start();

    const String& getVerboseName() const { return verbose_name; }
    const String& getMAC() const { return mac; }
    void publish(const String& partial_topic, const String& message);
};

#endif
