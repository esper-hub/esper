#include "WifiConnectionManager.h"
#include "util/Strings.h"


const Logger WifiConnectionManager::LOG = Logger("wifi");

WifiConnectionManager::WifiConnectionManager(const StateChangedCallback& callback) :
        state(State::DISCONNECTED, callback) {
    this->reconnectTimer.initializeMs(2000, TimerDelegate(&WifiConnectionManager::connect, this));

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

const String& WifiConnectionManager::getCurrentBSSID() const {
    return this->bssid;
}

void WifiConnectionManager::onStationConfigured(IPAddress ip, IPAddress mask, IPAddress gateway) {
    LOG.log("Configured:", ip, mask, gateway);

    if (this->state.set(State::CONNECTED)) {
        this->reconnectTimer.stop();
    }
}

void WifiConnectionManager::onStationConnected(String ssid, uint8_t, uint8_t bssid[6], uint8_t reason) {
    LOG.log("Connected");

    this->ssid = ssid;
    this->bssid = Strings::formatMAC(bssid);
}

void WifiConnectionManager::onStationDisconnected(String, uint8_t, uint8_t[6], uint8_t reason) {
    LOG.log("Disconnected");

    this->ssid = String();
    this->bssid = String();

    if (this->state.set(State::DISCONNECTED)) {
        this->reconnectTimer.start();
    }
}
