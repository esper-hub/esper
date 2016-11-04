#include "HassDevice.h"

extern const char HASS_DEVICE_LOG_NAME[] = "[HassDevice] ";


HassDevice::HassDevice(const String verbose_name) :
        verbose_name(verbose_name),
        wifiConnectionManager(WifiConnectionManager::StateChangedCallback(&HassDevice::onWifiStateChanged, this)),
        mqttConnectionManager(MqttConnectionManager::StateChangedCallback(&HassDevice::onMqttStateChanged, this),
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

void HassDevice::onWifiStateChanged(const WifiConnectionManager::State& state) {
    const char wifi_state_changed[] = "Wifi state changed to: ";
    switch (state) {
        case WifiConnectionManager::State::CONNECTED:
            log(wifi_state_changed, "CONNECTED");
            mqttConnect();
            break;
        case WifiConnectionManager::State::DISCONNECTED:
            log(wifi_state_changed, "DISCONNECTED");
            break;
        case WifiConnectionManager::State::CONNECTING:
            log(wifi_state_changed, "CONNECTING");
            break;
    }
}

void HassDevice::onMqttStateChanged(const MqttConnectionManager::State& state) {
    const char mqtt_state_changed[] = "Mqtt state changed to: ";
    switch (state) {
        case MqttConnectionManager::State::CONNECTED:
            log(mqtt_state_changed, "connected");
            onMqttConnected();
            break;
        case MqttConnectionManager::State::DISCONNECTED:
            log(mqtt_state_changed, "disconnected");
            break;
        case MqttConnectionManager::State::CONNECTING:
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
    if (mqttConnectionManager.getState() != MqttConnectionManager::State::CONNECTED)
        return;
    mqttConnectionManager.publish(basePath + partial_topic, message);
}

void HassDevice::add(AbstractFeature *feature) {
    logf("adding feature %p", feature);
    if (!feature) return;
    if (!features.contains(feature))
        features.addElement(feature);
}
