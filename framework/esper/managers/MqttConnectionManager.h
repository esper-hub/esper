#ifndef MQTT_CONNECTION_MANAGER_H
#define MQTT_CONNECTION_MANAGER_H

#include <SmingCore.h>

#include "../util/Logger.h"
#include "../util/Observed.h"


class MqttConnectionManager {
    static const Logger LOG;

    MqttConnectionManager(MqttConnectionManager const&) = delete;
    MqttConnectionManager& operator=(MqttConnectionManager const&) = delete;

public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;
    using MessageCallback = Delegate<void(const String& topic, const String& message)>;

    MqttConnectionManager(const StateChangedCallback& stateChangedCallback,
                          const MessageCallback& messageCallback);
    ~MqttConnectionManager();

    void setWill(const String& topic, const String& message, const bool& retained = false);

    void connect();

    void subscribe(const String &topic);

    void publish(const String &topic, const String &message, const bool& retain = false);

    State getState() const;

private:
    int onConnected(MqttClient& client, mqtt_message_t* message);
    void onDisconnected(TcpClient &client, bool flag);

    int onMessageReceived(MqttClient& client, mqtt_message_t* message);

    const String topicBase;

    Observed<State> state;

    MqttClient client;
    const MessageCallback messageCallback;

    Timer reconnectTimer;
};

#endif
