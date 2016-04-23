#include <user_config.h>
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

MqttNode::MqttNode(String wifiSsid, String wifiPwd, String mqttHost, int mqttPort) :
        wifiSsid(wifiSsid), wifiPwd(wifiPwd), mqttHost(mqttHost), mqttPort(mqttPort),
        mqtt(MqttClient(mqttHost, mqttPort,
                        MqttStringSubscriptionCallback(&MqttNode::onMessageReceived, this))) {
}

void MqttNode::connect() {
    Serial.println("connect()");
    WifiStation.config(wifiSsid, wifiPwd);
    WifiStation.enable(true);
    WifiAccessPoint.enable(false);

    WifiStation.waitConnection(ConnectionDelegate(&MqttNode::wifiConnectOk, this), 10,
                               ConnectionDelegate(&MqttNode::wifiConnectFail, this));
}

void MqttNode::wifiConnectFail() {
    Serial.println("WiFi connection received timeout. Reboot…");
    System.restart();
}

void MqttNode::wifiConnectOk() {
    Serial.println("wifiConnectOk()");
    startMqttClient();
}

void MqttNode::startMqttClient() {
    Serial.println("startMqttClient()");
    procTimer.stop();

    mqtt.connect(WifiStation.getMAC());
    // Assign a disconnect callback function
    mqtt.setCompleteDelegate(TcpClientCompleteDelegate(&MqttNode::checkMQTTDisconnect, this));

    //run callback
    onMqttConnected();
}


void MqttNode::checkMQTTDisconnect(TcpClient &client, bool flag) {
    // Called whenever MQTT connection is failed.
    if (flag)
        Serial.println("MQTT Broker Disconnected!!");
    else
        Serial.println("MQTT Broker Unreachable!!");

    // Restart connection attempt after few seconds
    procTimer.initializeMs(2 * 1000, TimerDelegate(&MqttNode::startMqttClient, this)).start(); // every 2 seconds
}
