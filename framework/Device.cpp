#include "Device.h"


ServiceBase::ServiceBase() {
}

ServiceBase::~ServiceBase() {
}


String calculateTopicBase() {
    char topicBase[sizeof(MQTT_REALM) + 6 + 1];
    sprintf(topicBase, "%s/%06x", MQTT_REALM, system_get_chip_id());
    return String(topicBase);
}


const Logger Device::LOG = Logger("device");

const String Device::TOPIC_BASE = calculateTopicBase();

Device::Device() :
        wifiConnectionManager(WifiConnectionManager::StateChangedCallback(&Device::onWifiStateChanged, this)),
        mqttConnectionManager(MqttConnectionManager::StateChangedCallback(&Device::onMqttStateChanged, this),
                              MqttConnectionManager::MessageCallback(&Device::onMqttMessageReceived, this)),
        ntpClient(NtpTimeResultDelegate(&Device::onTimeUpdated, this)),

#if HEARTBEAT_ENABLED
        heartbeat(this),
#endif
#if UPDATE_ENABLED
        update(this),
#endif
        info(this) {

#if HEARTBEAT_ENABLED
    this->add(&heartbeat);
#endif
#if UPDATE_ENABLED
    this->add(&update);
#endif
    this->add(&info);

    LOG.log("Initialized");
    LOG.log("Base Path:", Device::TOPIC_BASE);
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
    this->messageCallbacks[topic] = callback;
}

void Device::add(ServiceBase* const service) {
    if (!services.contains(service)) {
        services.addElement(service);
        LOG.log("Added service:", service->getName());
    }
}

const WifiConnectionManager& Device::getWifi() const {
    return this->wifiConnectionManager;
}

const MqttConnectionManager& Device::getMqtt() const {
    return this->mqttConnectionManager;
}

void Device::publish(const String& topic, const String& message, const bool& retain) {
    if (this->mqttConnectionManager.getState() != MqttConnectionManager::State::CONNECTED)
        return;

    this->mqttConnectionManager.publish(topic,
                                        message,
                                        retain);
}

void Device::onWifiStateChanged(const WifiConnectionManager::State& state) {
    switch (state) {
        case WifiConnectionManager::State::CONNECTED: {
            LOG.log("WiFi state changed: Connected");
            this->ntpClient.requestTime();
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

            // Subscribe for all known registered callbacks
            for (unsigned int i = 0; i < this->messageCallbacks.count(); i++) {
                const auto topic = this->messageCallbacks.keyAt(i);
                this->mqttConnectionManager.subscribe(topic);
            }

            // Inform all services about new state
            for (int i = 0; i < this->services.count(); i++) {
                this->services[i]->onStateChanged(ServiceBase::State::CONNECTED);
            }

            break;
        }

        case MqttConnectionManager::State::DISCONNECTED: {
            LOG.log("MQTT state changed: Disconnected");

            // Inform all services about new state
            for (int i = 0; i < this->services.count(); i++) {
                this->services[i]->onStateChanged(ServiceBase::State::DISCONNECTED);
            }

            break;
        }

        case MqttConnectionManager::State::CONNECTING: {
            LOG.log("MQTT state changed: Connecting");
            break;
        }
    }
}

void Device::onMqttMessageReceived(const String& topic, const String& message) {
    // Dispatch message to registered feature handler
    auto i = this->messageCallbacks.indexOf(topic);
    if (i != -1) {
        const auto& callback = this->messageCallbacks.valueAt(i);
        callback(topic.substring(Device::TOPIC_BASE.length()+1), message);
    }
}

void Device::onTimeUpdated(NtpClient& client, time_t curr) {
    auto prev = RTC.getRtcSeconds();
    RTC.setRtcSeconds(curr);

    LOG.log("Time updated:", DateTime(curr).toISO8601());

    if (abs(curr - prev) > 60 * 60) {
        LOG.log("Clock differs to much - rebooting");
        this->reboot();
    }
}


void init() {
    // Configure system
    System.setCpuFrequency(eCF_160MHz);

    // Initialize logging
    Serial.end();
    Logger::init();

    // Create the device and start it
    createDevice()->start();
}
