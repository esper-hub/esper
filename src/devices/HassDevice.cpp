#include "HassDevice.h"

extern const char HASS_DEVICE_LOG_NAME[] = "[HassDevice] ";


HassDevice::HassDevice(const String verbose_name) :
        verbose_name(verbose_name),
        wifiConnectionManager(
                WifiConnectionManager::WifiConnectionStateChangedDelegate(&HassDevice::onWifiStateChanged, this)),
        mqttConnectionManager(
                MqttConnectionManager::MqttConnectionStateChangedDelegate(&HassDevice::onMqttStateChanged, this),
                MqttStringSubscriptionCallback(&HassDevice::onMqttMessageReceived, this)),
        basePath(REALM "/" + WifiStation.getMAC() + "/") {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    log("initializing");
    Serial.systemDebugOutput(true); // Debug output to serial
    log("initialized");
}


void HassDevice::start() {
    log("starting");
    wifiConnectionManager.connect();
    log("started");
}

void HassDevice::onWifiStateChanged(WifiConnectionManager::WifiState state) {
    const char wifi_state_changed[] = "Wifi state changed to: ";
    switch (state) {
        case WifiConnectionManager::WifiState::CONNECTED:
            log(wifi_state_changed, "CONNECTED");
            mqttConnect();
            break;
        case WifiConnectionManager::WifiState::DISCONNECTED:
            log(wifi_state_changed, "DISCONNECTED");
            break;
        case WifiConnectionManager::WifiState::CONNECTING:
            log(wifi_state_changed, "CONNECTING");
            break;
    }
}

void HassDevice::onMqttStateChanged(MqttConnectionManager::MqttState state) {
    const char mqtt_state_changed[] = "Mqtt state changed to: ";
    switch (state) {
        case MqttConnectionManager::MqttState::CONNECTED:
            log(mqtt_state_changed, "connected");
            onMqttConnected();
            break;
        case MqttConnectionManager::MqttState::DISCONNECTED:
            log(mqtt_state_changed, "disconnected");
            break;
        case MqttConnectionManager::MqttState::CONNECTING:
            log(mqtt_state_changed, "connecting");
            break;
    }
}

void HassDevice::mqttConnect() {
    log("Connectiong to Mqtt Broker");
    mqttConnectionManager.connect();
}

void HassDevice::restart() {
    log("Restarting.");
    System.restart();
}

void HassDevice::onMqttConnected() {
    log("Mqtt connected. \\o/");
    for (int i = 0; i < features.count(); i++) {
        features[i]->onMqttConnected();
    }
    for (unsigned int i = 0; i < messageCallbacks.count(); i++) {
        const auto topic = messageCallbacks.keyAt(i);
        log("subscribing to: ", topic);
        mqttSubscribe(topic);
    }
}

void HassDevice::registerSubscription(const String topic, MqttStringSubscriptionCallback cb) {
    if (!topic) return;
    messageCallbacks[basePath + topic] = cb;
}

void HassDevice::mqttSubscribe(const String &topic) {
    mqttConnectionManager.subscribe(topic);
}

template<typename StringType>
static inline const StringType ltrim(const StringType &s, const StringType &token) {
    auto f = s.lastIndexOf(token);
    auto x = s.substring(f + token.length());
    return x;
}


void HassDevice::onMqttMessageReceived(String topic, String msg) {
    for (int i = 0; i < messageCallbacks.count(); i++) {
        auto key = messageCallbacks.keyAt(i);
        if(key == topic) {
            messageCallbacks[key](topic, msg);
            return;
        }
    }
}

void HassDevice::publish(const String &partial_topic, const String &message) {
    if (mqttConnectionManager.getState() != MqttConnectionManager::MqttState::CONNECTED)
        return;
    mqttConnectionManager.publish(basePath + partial_topic, message);
}

void HassDevice::add(AbstractFeature *feature) {
    logf("adding feature %p", feature);
    if (!feature) return;
    if (!features.contains(feature))
        features.addElement(feature);
}
