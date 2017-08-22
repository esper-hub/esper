#include "MqttConnectionManager.h"


const Logger MqttConnectionManager::LOG = Logger("mqtt");

MqttConnectionManager::MqttConnectionManager(const StateChangedCallback& stateChangedCallback,
                                             const MessageCallback& messageCallback) :
        state(State::DISCONNECTED, stateChangedCallback),
        client(MQTT_HOST, MQTT_PORT,
               MqttStringSubscriptionCallback(&MqttConnectionManager::onMessageReceived, this)),
        messageCallback(messageCallback) {
    this->reconnectTimer.initializeMs(2000, TimerDelegate(&MqttConnectionManager::connect, this));

    LOG.log("Initialized");
}

MqttConnectionManager::~MqttConnectionManager() {
}

void MqttConnectionManager::connect() {
    LOG.log("Connecting");

    this->reconnectTimer.stop();
    this->state.set(State::CONNECTING);

    this->client.setCompleteDelegate(TcpClientCompleteDelegate(&MqttConnectionManager::onDisconnected, this));
    if (this->client.connect(WifiStation.getMAC(), MQTT_USERNAME, MQTT_PASSWORD)) {
        this->state.set(State::CONNECTED);

    } else {
        this->state.set(State::DISCONNECTED);

        LOG.log("Failed to connect - reconnecting");
        this->reconnectTimer.start();
    }
}

void MqttConnectionManager::subscribe(const String &topic) {
    this->client.subscribe(topic);
    LOG.log("Subscribed to:", topic);
}

void MqttConnectionManager::publish(const String &topic, const String &message, const bool& retain) {
    LOG.log("Publishing message to", topic, ":", message);
    this->client.publish(topic, message, retain);
}

void MqttConnectionManager::setWill(const String& topic, const String& message, const bool& retained) {
    LOG.log("Setlast will to ", topic, ":", message);
	this->client.setWill(topic, message, 0, retained);
}

MqttConnectionManager::State MqttConnectionManager::getState() const {
    return this->state;
}

void MqttConnectionManager::onDisconnected(TcpClient &client, bool flag) {
    if (flag) {
        LOG.log("Disconnected");
    } else {
        LOG.log("Unreachable");
    }

    this->state.set(State::DISCONNECTED);
    this->reconnectTimer.start();
}

void MqttConnectionManager::onMessageReceived(const String topic, const String message) {
    LOG.log("Received topic:", topic);
    LOG.log("Received message:", message);

    this->messageCallback(topic, message);
}
