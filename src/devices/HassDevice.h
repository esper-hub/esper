#ifndef HASSDEVICE_HPP
#define HASSDEVICE_HPP

#include <SmingCore/SmingCore.h>
#include <user_config.h>
#include "../managers/WifiConnectionManager.h"
#include "../managers/MqttConnectionManager.h"
#include "../features/AbstractFeature.h"


extern const char HASS_DEVICE_LOG_NAME[];

class HassDevice : private Log<HASS_DEVICE_LOG_NAME> {
    const String verbose_name;
    const String mac;
    const String basePath;

    WifiConnectionManager wifiConnectionManager;
    MqttConnectionManager mqttConnectionManager;

    HashMap <String, MqttStringSubscriptionCallback> messageCallbacks;
    Vector<AbstractFeature *> features;

private:
    void mqttConnect();

    void onWifiStateChanged(const WifiConnectionManager::State& state);

    void onMqttStateChanged(const MqttConnectionManager::State& state);

    void onMqttMessageReceived(String topic, String message);

    void onMqttConnected();

    void mqttSubscribe(const String &topic);

public:
    HassDevice(const String verbose_name);

    void start();

    void restart();

    void registerSubscription(const String topic, MqttStringSubscriptionCallback cb);


    void add(AbstractFeature *feature);

    template<typename FeatureType, typename ...Args>
    void add(Args... args) {
        logf("no hope left.");
        FeatureType *feature = new FeatureType(*this, args...);
        logf("constructed new feature: %p", feature);
        add(feature);
        logf("feature added.");
    };


    const String &getVerboseName() const { return verbose_name; }

    const String &getMAC() const { return mac; }

    void publish(const String &partial_topic, const String &message);
};

#endif
