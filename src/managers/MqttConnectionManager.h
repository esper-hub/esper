#ifndef MQTT_CONNECTION_MANAGER_H
#define MQTT_CONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>

#include "../util/Logger.h"
#include "../util/Observed.h"


class MqttConnectionManager {
    static const Logger LOG;

public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;
    using MessageCallback = Delegate<void(const String& topic, const String& message)> ;

    MqttConnectionManager(StateChangedCallback cb,
                          MessageCallback messageCallback);

    void connect();

    void subscribe(const String &topic);

    void publish(const String &topic, const String &message);

    State getState() const;

private:
    void onDisconnected(TcpClient &client, bool flag);

    void startReconnectTimer();

    void onMessageReceived(const String topic, const String message);

    Observed<State> state;

    MqttClient client;
    MessageCallback messageCallback;

    Timer reconnectTimer;
};

#endif
