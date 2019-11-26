#include "MqttConnectionManager.h"


const Logger MqttConnectionManager::LOG = Logger("mqtt");

MqttConnectionManager::MqttConnectionManager(const StateChangedCallback& stateChangedCallback,
                                             const MessageCallback& messageCallback) :
        state(State::DISCONNECTED, stateChangedCallback),
        messageCallback(messageCallback) {
    this->client.setConnectedHandler(MqttDelegate(&MqttConnectionManager::onConnected, this));
    this->client.setDisconnectHandler(TcpClientCompleteDelegate(&MqttConnectionManager::onDisconnected, this));

    this->client.setMessageHandler(MqttDelegate(&MqttConnectionManager::onMessageReceived, this));

    this->reconnectTimer.initializeMs(2000, std::bind(&MqttConnectionManager::connect, this));

    LOG.log(F("Initialized"));
}

MqttConnectionManager::~MqttConnectionManager() {
}

void MqttConnectionManager::connect() {
    LOG.log(F("Connecting:"), MQTT_URL);

    this->reconnectTimer.stop();
    this->state.set(State::CONNECTING);

    if (!this->client.connect(MQTT_URL, WifiStation.getMAC())) {
        this->state.set(State::DISCONNECTED);

        LOG.log(F("Failed to connect - reconnecting"));
        this->reconnectTimer.start();
    }
}

void MqttConnectionManager::subscribe(const String &topic) {
    this->client.subscribe(topic);
    LOG.log(F("Subscribed to:"), topic);
}

void MqttConnectionManager::publish(const String &topic, const String &message, const bool& retain) {
    LOG.log(F("Publishing message to"), topic, F(":"), message);
    this->client.publish(topic, message, retain);
}

void MqttConnectionManager::setWill(const String& topic, const String& message, const bool& retained) {
    LOG.log(F("Set last will to"), topic, F(":"), message);
    this->client.setWill(topic, message, 0, retained);
}

MqttConnectionManager::State MqttConnectionManager::getState() const {
    return this->state;
}

int MqttConnectionManager::onConnected(MqttClient& client, mqtt_message_t* message) {
        this->reconnectTimer.stop();
        this->state.set(State::CONNECTED);
        
        LOG.log(F("Connected"));

        return 0;
}

void MqttConnectionManager::onDisconnected(TcpClient &client, bool flag) {
    if (flag) {
        LOG.log(F("Disconnected"));
    } else {
        LOG.log(F("Unreachable"));
    }

    if (this->state == State::CONNECTED) {
        this->state.set(State::DISCONNECTED);
        this->reconnectTimer.start();
    } else {
        LOG.log(F("Error: Ignored disconnect while connecting"));
    }
}

int MqttConnectionManager::onMessageReceived(MqttClient& client, mqtt_message_t* message) {
    if (message == nullptr) {
        return -1;
    }

    const String topic = String((const char*)message->publish.topic_name.data, message->publish.topic_name.length);
    const String payload = String((const char*)message->publish.content.data, message->publish.content.length);

    LOG.log(F("Received message:"), topic, F("="), payload);

    System.queueCallback([=]() {
        this->messageCallback(topic, payload);
    });

    return 0;
}
