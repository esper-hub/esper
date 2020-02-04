#include "Device.h"
#include <stdio.h>


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
        info(this),
        reboot(this) {

#if HEARTBEAT_ENABLED
    this->add(&this->heartbeat);
#endif
#if UPDATE_ENABLED
    this->add(&this->update);
#endif
    this->add(&this->info);
    this->add(&this->reboot);

    LOG.log(F("Initialized"));
    LOG.log(F("Base Path:"), Device::TOPIC_BASE);
}

Device::~Device() {
    LOG.log(F("Something's happening in here!"));
}

void Device::start() {
    LOG.log(F("Starting"));

    // Set last will to publish status as offline
    this->mqttConnectionManager.setWill(Device::TOPIC_BASE + F("/status"), F("OFFLINE"), true);

    // Start connecting to the network
    this->wifiConnectionManager.connect();

#if HTTP_ENABLED
    this->http.listen(HTTP_PORT);
#endif

    LOG.log(F("Started"));
}

void Device::triggerReboot(unsigned int deferMillis) {
    this->reboot.trigger(deferMillis);
}

void Device::registerSubscription(const String& topic, const MessageCallback& callback) {
    LOG.log(F("Registering subscription:"), topic);
    this->messageCallbacks[topic] = callback;
}

#if HTTP_ENABLED
void Device::registerResource(const String& path, HttpResource* resource) {
    LOG.log(F("Registering resource:"), path);
    this->http.paths.set(path, resource);
}
#endif

void Device::add(ServiceBase* const service) {
    if (!services.contains(service)) {
        services.addElement(service);
        LOG.log(F("Added service:"), service->getName());
    }
}

const WifiConnectionManager& Device::getWifi() const {
    return this->wifiConnectionManager;
}

const MqttConnectionManager& Device::getMqtt() const {
    return this->mqttConnectionManager;
}

const Vector<ServiceBase*>& Device::getServices() const {
    return this->services;
}

const HashMap<String, Device::MessageCallback>& Device::getSubscriptions() const {
    return this->messageCallbacks;
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
            LOG.log(F("WiFi state changed: Connected"));
            this->ntpClient.requestTime();
            this->mqttConnectionManager.connect();
            break;
        }

        case WifiConnectionManager::State::DISCONNECTED: {
            LOG.log(F("WiFi state changed: Disconnected"));
            break;
        }

        case WifiConnectionManager::State::CONNECTING: {
            LOG.log(F("WiFi state changed: Connecting"));
            break;
        }
    }
}

void Device::onMqttStateChanged(const MqttConnectionManager::State& state) {
    switch (state) {
        case MqttConnectionManager::State::CONNECTED: {
            LOG.log(F("MQTT state changed: Connected \\o/"));

            // Publish status as online
            this->mqttConnectionManager.publish(Device::TOPIC_BASE + F("/status"), F("ONLINE"), true);

            // Subscribe for all known registered callbacks
            for (unsigned int i = 0; i < this->messageCallbacks.count(); i++) {
                const auto topic = this->messageCallbacks.keyAt(i);
                this->mqttConnectionManager.subscribe(topic);
            }

            // Inform all services about new state
            for (unsigned int i = 0; i < this->services.count(); i++) {
                this->services[i]->onStateChanged(ServiceBase::State::CONNECTED);
            }

            break;
        }

        case MqttConnectionManager::State::DISCONNECTED: {
            LOG.log(F("MQTT state changed: Disconnected"));

            // Inform all services about new state
            for (unsigned int i = 0; i < this->services.count(); i++) {
                this->services[i]->onStateChanged(ServiceBase::State::DISCONNECTED);
            }

            break;
        }

        case MqttConnectionManager::State::CONNECTING: {
            LOG.log(F("MQTT state changed: Connecting"));
            break;
        }
    }
}

void Device::onMqttMessageReceived(const String& topic, const String& message) {
    LOG.log(F("MQTT message received:"), topic, F(":"), message);

    // Dispatch message to registered handlers
    auto i = this->messageCallbacks.indexOf(topic);
    if (i != -1) {
        const auto& callback = this->messageCallbacks.valueAt(i);
        System.queueCallback([=]() {
            callback(message);
        });
    }
}

void Device::onTimeUpdated(NtpClient& client, time_t curr) {
    auto prev = RTC.getRtcSeconds();
    RTC.setRtcSeconds(curr);

    LOG.log(F("Time updated:"), DateTime(curr).toISO8601());

    if (abs(curr - prev) > 60 * 60) {
        LOG.log(F("Clock differs to much - rebooting"));
        this->triggerReboot();
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
