#include "WifiConnectionManager.h"


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
    WifiEvents.onStationDisconnect(StationDisconnectDelegate(&WifiConnectionManager::onStationDisconnected, this));
}

WifiConnectionManager::State WifiConnectionManager::getState() const {
    return this->state;
}


void WifiConnectionManager::onStationConfigured(IPAddress ip, IPAddress mask, IPAddress gateway) {
    LOG.log("Configured:", ip, mask, gateway);

    if (this->state.set(State::CONNECTED)) {
        this->reconnectTimer.stop();
    }
}

void WifiConnectionManager::onStationDisconnected(String ssid, uint8_t ssidLength, uint8_t bssid[6], uint8_t reason) {
    LOG.log("Disconnected");

    if (this->state.set(State::DISCONNECTED)) {
        this->reconnectTimer.start();
    }
}
