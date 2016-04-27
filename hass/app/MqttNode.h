#ifndef MQTTNODE_H
#define MQTTNODE_H

#include <SmingCore/SmingCore.h>


class MqttNode {
    String wifiSsid, wifiPwd, mqttHost;
    int mqttPort;
    MqttClient mqtt;
    Timer procTimer;
public:
    MqttNode(String wifiSsid, String wifiPwd, String mqttHost, int mqttPort);

    void connect();

    // Callback after mqtt connection is (re?)-established
    virtual void onMqttConnected() = 0;

    // Callback for messages, arrived from MQTT server
    virtual void onMessageReceived(String topic, String message) = 0;

    void publish(String topic, String message, bool retained = false) {
        mqtt.publish(topic, message, retained);
    }

    void subscribe(String topic) {
        mqtt.subscribe(topic);
    }

    void unsubscribe(String topic) {
        mqtt.unsubscribe(topic);
    }

    /**
     * set a last will that will be published by the broker if the node gets disconnected ungracefully.
     * this needs to be set before connecting to the broker
     */
    void setWill(String topic, String message, int QoS, bool retained = false) {
        mqtt.setWill(topic, message, QoS, retained);
    }

private:
    void startMqttClient();

    void wifiConnectOk();

    void wifiConnectFail();

    void checkMQTTDisconnect(TcpClient &client, bool flag);

};


#endif // MQTTNODE_H
