#include "Device.h"


FeatureBase::FeatureBase() {
}

FeatureBase::~FeatureBase() {
}


const Logger Device::LOG = Logger("Device");

Device::Device() :
        wifiConnectionManager(WifiConnectionManager::StateChangedCallback(&Device::onWifiStateChanged, this)),
        mqttConnectionManager(MqttConnectionManager::StateChangedCallback(&Device::onMqttStateChanged, this),
                              MqttConnectionManager::MessageCallback(&Device::onMqttMessageReceived, this)),
        topicBase(MQTT_REALM + ("/" + String(system_get_chip_id(), 16))) {
#ifdef MQTT_HEARTBEAT
    // Reboot the system if heartbeat was missing
    this->heartbeatTimer.initializeMs(120000, TimerDelegate(&Device::reboot, this));
#endif

    LOG.log("Initialized");
    LOG.log("Base Path:", this->topicBase);
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
    this->messageCallbacks[topicBase + ("/" + topic)] = callback;
}

void Device::add(FeatureBase* feature) {
    if (!features.contains(feature)) {
        features.addElement(feature);
        feature->registerSubscriptions();
        LOG.log("Added feature:", feature->getName());
    }
}

void Device::publish(const String& topic, const String& message) {
    if (this->mqttConnectionManager.getState() != MqttConnectionManager::State::CONNECTED)
        return;

    this->mqttConnectionManager.publish(topicBase + topic, message);
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

            this->mqttConnectionManager.publish(this->topicBase,
                                                StringSumHelper("") +
                                                "ESPER=v" + VERSION + "\n" +
                                                        "DEVICE=" +
                                                "SDK=v" + system_get_sdk_version() + "\n" +
                                                "BOOT=v" + String(system_get_boot_version()) + "\n" +
                                                "CHIP=" + String(system_get_chip_id(), 16) + "\n" +
                                                "FLASH=" + String(spi_flash_get_id(), 16) + "\n");

#ifdef MQTT_HEARTBEAT
            // Start awaiting heartbeats
            this->mqttConnectionManager.subscribe(MQTT_REALM "/heartbeat");
            this->heartbeatTimer.start();
#endif

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

#ifdef MQTT_HEARTBEAT
            // Heartbeats are likely to miss if disconnected
            this->heartbeatTimer.stop();
#endif

            break;
        }

        case MqttConnectionManager::State::CONNECTING: {
            LOG.log("MQTT state changed: Connecting");
            break;
        }
    }
}

void Device::onMqttMessageReceived(const String& topic, const String& message) {

#ifdef MQTT_HEARTBEAT
    if (topic == MQTT_REALM "/heartbeat") {
        // Handle incoming heartbeat
        LOG.log("Heartbeat");
        this->heartbeatTimer.restart();
#else
    if (false) {
#endif

    } else {
        // Dispatch message to registered feature handler
        auto i = this->messageCallbacks.indexOf(topic);
        if (i != -1) {
            const auto& callback = this->messageCallbacks.valueAt(i);
            callback(topic, message);
        }
    }
}


void init() {
    System.setCpuFrequency(eCF_160MHz);

    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(true); // Debug output to serial

    Serial.printf("\r\n");
    Serial.printf("\r\n");
    Serial.printf("SDK: v%s\r\n", system_get_sdk_version());
    Serial.printf("Boot: v%u (%u)\r\n", system_get_boot_version(), system_get_boot_mode());
    Serial.printf("Free Heap: %d\r\n", system_get_free_heap_size());
    Serial.printf("CPU Frequency: %d MHz\r\n", system_get_cpu_freq());
    Serial.printf("System Chip ID: %x\r\n", system_get_chip_id());
    Serial.printf("SPI Flash ID: %x\r\n", spi_flash_get_id());
    Serial.printf("\r\n");
    Serial.printf("\r\n");

    Device* device = createDevice();
    device->start();
}
