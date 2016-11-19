#include "WifiConnectionManager.h"


const Logger WifiConnectionManager::LOG = Logger("WiFi");

WifiConnectionManager::WifiConnectionManager(const StateChangedCallback& callback) :
        state(State::DISCONNECTED, callback) {
    this->reconnectTimer.initializeMs(2000, TimerDelegate(&WifiConnectionManager::connect, this));

    LOG.log("Initalized.");
}

void WifiConnectionManager::connect() {
    LOG.log("Connecting");

    this->reconnectTimer.stop();

    LOG.log("SSID: ", WIFI_SSID);
    LOG.log("PW: ", WIFI_PWD);
    WifiStation.config(WIFI_SSID, WIFI_PWD);
    LOG.log("Configured");

    WifiAccessPoint.enable(false);
    LOG.log("Wifi Access Point disabled");

    WifiStation.enable(true);
    LOG.log("Wifi Station enabled");

    LOG.log("Waiting for connection");
    WifiStation.waitConnection(
            ConnectionDelegate(&WifiConnectionManager::onConnectOk, this), 10,
            ConnectionDelegate(&WifiConnectionManager::onConnectFail, this));

    this->state.set(State::CONNECTING);
}

WifiConnectionManager::State WifiConnectionManager::getState() const {
    return this->state;
}


void WifiConnectionManager::onConnectOk() {
    if (this->state.set(State::CONNECTED)) {
        this->reconnectTimer.stop();
    }
}

void WifiConnectionManager::onConnectFail() {
    if (this->state.set(State::DISCONNECTED)) {
        this->reconnectTimer.start();
    }
}
