#include "HassDevice.h"


FeatureBase::FeatureBase() {
}

FeatureBase::~FeatureBase() {
}


const Logger HassDevice::LOG = Logger("Hass Device");

HassDevice::HassDevice() :
        wifiConnectionManager(WifiConnectionManager::StateChangedCallback(&HassDevice::onWifiStateChanged, this)),
        mqttConnectionManager(MqttConnectionManager::StateChangedCallback(&HassDevice::onMqttStateChanged, this),
                              MqttConnectionManager::MessageCallback(&HassDevice::onMqttMessageReceived, this)) {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(true); // Debug output to serial

    LOG.log("Initialized");
}

HassDevice::~HassDevice() {
}

void HassDevice::start() {
    this->wifiConnectionManager.connect();

    LOG.log("Started");
}

void HassDevice::reboot() {
    LOG.log("Restarting System.");
    System.restart();
}

void HassDevice::registerSubscription(const String& topic, MessageCallback callback) {
    this->messageCallbacks[basePath + topic] = callback;
}

void HassDevice::add(FeatureBase* feature) {
    if (!features.contains(feature)) {
        features.addElement(feature);
        LOG.log("Added feature:", feature->getName());
    }
}

void HassDevice::publish(const String &partial_topic, const String &message) {
    if (mqttConnectionManager.getState() != MqttConnectionManager::State::CONNECTED)
        return;

    mqttConnectionManager.publish(basePath + partial_topic, message);
}

void HassDevice::onWifiStateChanged(const WifiConnectionManager::State& state) {
    switch (state) {
        case WifiConnectionManager::State::CONNECTED: {
            LOG.log("WiFi state changed: ", "CONNECTED");
            this->mqttConnectionManager.connect();
            break;
        }

        case WifiConnectionManager::State::DISCONNECTED: {
            LOG.log("WiFi state changed: ", "DISCONNECTED");
            break;
        }

        case WifiConnectionManager::State::CONNECTING: {
            LOG.log("WiFi state changed: ", "CONNECTING");
            break;
        }
    }
}

void HassDevice::onMqttStateChanged(const MqttConnectionManager::State& state) {
    switch (state) {
        case MqttConnectionManager::State::CONNECTED: {
            LOG.log("MQTT state changed: ", "connected \\o/");

            for (int i = 0; i < this->features.count(); i++) {
                this->features[i]->publishCurrentState();
            }

            for (unsigned int i = 0; i < this->messageCallbacks.count(); i++) {
                const auto topic = this->messageCallbacks.keyAt(i);
                this->mqttConnectionManager.subscribe(topic);
            }

            break;
        }

        case MqttConnectionManager::State::DISCONNECTED: {
            LOG.log("MQTT state changed: ", "disconnected");
            break;
        }

        case MqttConnectionManager::State::CONNECTING: {
            LOG.log("MQTT state changed: ", "connecting");
            break;
        }
    }
}

void HassDevice::onMqttMessageReceived(const String& topic, const String& message) {
    auto i = this->messageCallbacks.indexOf(topic);
    if (i != -1) {
        auto callback = this->messageCallbacks.valueAt(i);
        callback(topic, message);
    }
}
