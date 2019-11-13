#include "WifiConnectionManager.h"
#include "../util/Strings.h"


const Logger WifiConnectionManager::LOG = Logger("wifi");

WifiConnectionManager::WifiConnectionManager(const StateChangedCallback& callback) :
        state(State::DISCONNECTED, callback) {
    this->reconnectTimer.initializeMs(2000, std::bind(&WifiConnectionManager::connect, this));

    LOG.log("Initalized");
}

WifiConnectionManager::~WifiConnectionManager() {
}

void WifiConnectionManager::connect() {
    LOG.log("Connecting");

    this->reconnectTimer.stop();

    LOG.log("SSID:", WIFI_SSID);
    LOG.log("PW:", WIFI_PWD);
    WifiStation.config(WIFI_SSID, WIFI_PWD);
    LOG.log("Configured");

    WifiAccessPoint.enable(false);
    LOG.log("Wifi Access Point disabled");

    WifiStation.enable(true);
    LOG.log("Wifi Station enabled");

    this->state.set(State::CONNECTING);

    LOG.log("Waiting for connection");
    WifiEvents.onStationGotIP(StationGotIPDelegate(&WifiConnectionManager::onStationConfigured, this));
    WifiEvents.onStationConnect(StationConnectDelegate(&WifiConnectionManager::onStationConnected, this));
    WifiEvents.onStationDisconnect(StationDisconnectDelegate(&WifiConnectionManager::onStationDisconnected, this));
}

WifiConnectionManager::State WifiConnectionManager::getState() const {
    return this->state;
}

const String& WifiConnectionManager::getCurrentSSID() const {
    return this->ssid;
}

const MacAddress& WifiConnectionManager::getCurrentBSSID() const {
    return this->bssid;
}

void WifiConnectionManager::onStationConfigured(IpAddress ip, IpAddress mask, IpAddress gateway) {
    LOG.log("Configured:", ip, mask, gateway);

    if (this->state.set(State::CONNECTED)) {
        this->reconnectTimer.stop();
    }
}

void WifiConnectionManager::onStationConnected(const String& ssid, MacAddress bssid, uint8_t channel) {
    LOG.log("Connected");

    this->ssid = ssid;
    this->bssid = bssid;
}

void WifiConnectionManager::onStationDisconnected(const String& ssid, MacAddress bssid, WifiDisconnectReason reason) {
    LOG.log("Disconnected");

    this->ssid = String();
    this->bssid = MacAddress();

    if (this->state.set(State::DISCONNECTED)) {
        this->reconnectTimer.start();
    }
}
