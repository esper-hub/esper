#include "MqttNode.h"

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
