#include "HassDevice.h"
extern const char HASS_DEVICE_LOG_NAME[] = "[HassDevice] ";

template<typename A, typename B>
String make_path(const A base, const B local) {
	String tmp = base;
	tmp += "/";
	tmp += local;
	return tmp;
}


HassDevice::HassDevice(const String verbose_name) :
		verbose_name(verbose_name),
		wifiConnectionManager(WifiConnectionManager::WifiConnectionStateChangedDelegate(&HassDevice::onWifiStateChanged, this)),
		mqttConnectionManager(MqttConnectionManager::MqttConnectionStateChangedDelegate(&HassDevice::onMqttStateChanged, this),
			MqttStringSubscriptionCallback(&HassDevice::onMqttMessageReceived, this)),
		basePath(make_path(REALM, WifiStation.getMAC()))
{
        Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	log("initializing");
	Serial.systemDebugOutput(true); // Debug output to serial
	log("initialized");
}


void HassDevice::start() {
	log("starting");

	wifiConnectionManager.connect();

	log("started");
}

void HassDevice::connectWifi() {
	wifiConnectionManager.connect();
}

void HassDevice::onWifiStateChanged(WifiConnectionManager::WifiState state) {
	const char wifi_state_changed[] = "Wifi state changed to: ";
	switch (state) {
		case WifiConnectionManager::WifiState::CONNECTED:
			log(wifi_state_changed, "CONNECTED");
			mqttConnect();
			break;
		case WifiConnectionManager::WifiState::DISCONNECTED:
			log(wifi_state_changed, "DISCONNECTED");
			break;
		case WifiConnectionManager::WifiState::CONNECTING:
			log(wifi_state_changed, "CONNECTING");
			break;
		default:
			log("Are you shitting me??!");
	}
}

void HassDevice::onMqttStateChanged(MqttConnectionManager::MqttState state) {
	const char mqtt_state_changed[] = "Mqtt state changed to: ";
	switch (state) {
		case MqttConnectionManager::MqttState::CONNECTED:
			log(mqtt_state_changed, "connected");
			onMqttConnected();
			break;
		case MqttConnectionManager::MqttState::DISCONNECTED:
			log(mqtt_state_changed, "disconnected");
			break;
		case MqttConnectionManager::MqttState::CONNECTING:
			log(mqtt_state_changed, "connecting");
			break;
		default:
			log("Are you shitting me??!?");
	}
}

void HassDevice::mqttConnect() {
	log("Connectiong to Mqtt Broker");
	mqttConnectionManager.connect();	
}

void HassDevice::restart() {
	log("Restarting.");
	System.restart();
}

void HassDevice::onMqttConnected() {
	log("Mqtt connected. \\o/");
	for (int i = 0; i < features.count(); i++) {
		feature[i]->onMqttConnected();
	}
}

void HassDevice::registerSubscription(const String topic, MqttStringSubscriptionCallback cb) {
	if (!topic) return;
	if (messageCallbacks.contains(topic)) {
		messageCallbacks[topic] = cb;
	} else {
		messageCallbacks[topic] = cb;
		mqttSubscribe(topic);
	}
}

void HassDevice::mqttSubscribe(const String& topic) {
	const String path = basePath + "/" + topic;
	mqttConnectionManager.subscribe(path);
}

template<typename StringType>
static inline const StringType ltrim(const StringType& s, const StringType& token) {
	auto f = s.lastIndexOf(token);
	auto x = s.substring(f + token.length());
	return x;
}


void HassDevice::onMqttMessageReceived(String topic, String msg) {
	const String suffix = ltrim(topic, basePath); 
	
	for (int i = 0; i < messageCallbacks.count(); i++) {
		auto key = messageCallbacks.keyAt(i);
		if (key == suffix) {
			messageCallbacks[key](suffix, msg);
			return;
		}
	}
}

void HassDevice::publish(const String& partial_topic, const String& message) {
	const String topic = make_path(basePath, partial_topic);
	mqttConnectionManager.publish(topic, message);
}

void HassDevice::registerFeature(Feature* feature) {
	if (!feature) return;
	features.addElement(feature);
}
