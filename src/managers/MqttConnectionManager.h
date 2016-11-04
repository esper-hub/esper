#ifndef MQTT_CONNECTION_MANAGER_H
#define MQTT_CONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>

#include "../util/Log.h"
#include "../util/Observed.h"


extern const char MQTT_CONNECTION_MANAGER_LOG_NAME[];

class MqttConnectionManager : public Log<MQTT_CONNECTION_MANAGER_LOG_NAME> {
public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;

    MqttConnectionManager(StateChangedCallback cb,
                          MqttStringSubscriptionCallback messageCallback) :
            state(State::DISCONNECTED, cb),
            client(MQTT_HOST, MQTT_PORT,
                   MqttStringSubscriptionCallback(&MqttConnectionManager::onMessageReceived, this)),
            messageCallback(messageCallback) {
        log("intialized.");
    }

    void connect() {
        log("Connecting");
        reconnectTimer.stop();
        this->state.set(State::CONNECTING);
        client.setCompleteDelegate(TcpClientCompleteDelegate(&MqttConnectionManager::onDisconnected, this));
        if (client.connect(WifiStation.getMAC())) {
            this->state.set(State::CONNECTED);
        } else {
            this->state.set(State::DISCONNECTED);
            log("failed to connect.");
            startReconnectTimer();
        }
    }

    void subscribe(const String &topic) {
        log("Subscribed to: ", topic);
        client.subscribe(topic);
    }

    inline void publish(const String &topic, const String &message) {
        client.publish(topic, message);
    }

    inline State getState() const {
        return this->state;
    }

private:
    void onDisconnected(TcpClient &client, bool flag) {
        if (flag) {
            log("Disconnected.");
        } else {
            log("Unreachable.");
        }
        this->state.set(State::DISCONNECTED);
        startReconnectTimer();
    }

    void startReconnectTimer() {
        log("starting reconnect timer.");
        reconnectTimer.initializeMs(2000, TimerDelegate(&MqttConnectionManager::connect, this)).start();
    }

    void onMessageReceived(const String topic, const String message) {
        log("Recieved topic: ", topic);
        log("Received message: ", message);
        messageCallback(topic, message);
    }

    Observed<State> state;

    MqttClient client;
    MqttStringSubscriptionCallback messageCallback;

    Timer reconnectTimer;
};

#endif
