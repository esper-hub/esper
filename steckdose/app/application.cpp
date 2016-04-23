#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include "MqttNode.cpp"

#define LED_PIN 2 // GPIO2

class LEDMqttNode : public MqttNode {
public:
    using MqttNode::MqttNode;

    void onMqttConnected() {
        Serial.println("onMqttConnected2");
        subscribe("/rq");
    }

    void onMessageReceived(String topic, String message) {
        Serial.println("onMessageReceived2");
        Serial.print(topic);
        Serial.print(": "); // Prettify alignment for printing
        Serial.println(message);

        if (topic == "/rq") {
            blinkTmr.stop();

            if(message == "0" || message == "1" || message == "toggle") {
                if (message == "1")
                    state = 1;
                else if (message == "0")
                    state = 0;
                else
                    state = !state;

                //LED is inverted (lights up on 0)
                digitalWrite(LED_PIN, !state);
            }

            else if(message == "blink") {
                blinkTmr.initializeMs(1000, TimerDelegate(&LEDMqttNode::blink, this)).start();
            }

            else {
                Serial.print("Unknown request: ");
                Serial.println(message);
            }
        }
    }

private:
    Timer blinkTmr;
    bool state = 0;

    void blink() {
        state = !state;
        digitalWrite(LED_PIN, state);
    }
};



void init() {
    Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
    Serial.systemDebugOutput(true); // Debug output to serial

    Serial.println(WIFI_SSID);
    Serial.println(WIFI_PWD);
    Serial.println(MQTT_HOST);
    Serial.println(MQTT_PORT);

    LEDMqttNode ledMqttNode(WIFI_SSID, WIFI_PWD, MQTT_HOST, MQTT_PORT);
    //configure LED_PIN for outputs
    pinMode(LED_PIN, OUTPUT);
    //LED is inverted (lights up on 0)
    digitalWrite(LED_PIN, 1);

    ledMqttNode.setWill("/foo", "offline", 1);
    ledMqttNode.connect();
}
