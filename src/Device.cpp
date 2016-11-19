#include "Device.h"


FeatureBase::FeatureBase() {
}

FeatureBase::~FeatureBase() {
}


const Logger Device::LOG = Logger("Device");

Device::Device() :
        wifiConnectionManager(WifiConnectionManager::StateChangedCallback(&Device::onWifiStateChanged, this)),
        mqttConnectionManager(MqttConnectionManager::StateChangedCallback(&Device::onMqttStateChanged, this),
                              MqttConnectionManager::MessageCallback(&Device::onMqttMessageReceived, this)) {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(true); // Debug output to serial

    LOG.log("Initialized");
}

Device::~Device() {
}

void Device::start() {
    LOG.log("Starting");

    this->wifiConnectionManager.connect();

    LOG.log("Started");
}

void Device::reboot() {
    LOG.log("Restarting System");
    System.restart();
}

void Device::registerSubscription(const String& topic, const MessageCallback& callback) {
    this->messageCallbacks[basePath + topic] = callback;
}

void Device::add(FeatureBase* feature) {
    if (!features.contains(feature)) {
        features.addElement(feature);
        LOG.log("Added feature:", feature->getName());
    }
}

void Device::publish(const String &partial_topic, const String &message) {
    if (mqttConnectionManager.getState() != MqttConnectionManager::State::CONNECTED)
        return;

    mqttConnectionManager.publish(basePath + partial_topic, message);
}

void Device::onWifiStateChanged(const WifiConnectionManager::State& state) {
    switch (state) {
        case WifiConnectionManager::State::CONNECTED: {
            LOG.log("WiFi state changed: Connected");
            this->mqttConnectionManager.connect();
            break;
        }

        case WifiConnectionManager::State::DISCONNECTED: {
            LOG.log("WiFi state changed: Disconnected");
            break;
        }

        case WifiConnectionManager::State::CONNECTING: {
            LOG.log("WiFi state changed: Connecting");
            break;
        }
    }
}

void Device::onMqttStateChanged(const MqttConnectionManager::State& state) {
    switch (state) {
        case MqttConnectionManager::State::CONNECTED: {
            LOG.log("MQTT state changed: Connected \\o/");

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
            LOG.log("MQTT state changed: Disconnected");
            break;
        }

        case MqttConnectionManager::State::CONNECTING: {
            LOG.log("MQTT state changed: Connecting");
            break;
        }
    }
}

void Device::onMqttMessageReceived(const String& topic, const String& message) {
    auto i = this->messageCallbacks.indexOf(topic);
    if (i != -1) {
        auto callback = this->messageCallbacks.valueAt(i);
        callback(topic, message);
    }
}


void init() {
    extern Device* device;
    device->start();
}
