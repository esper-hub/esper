#include "WifiConnectionManager.h"
#include "../util/Strings.h"


const Logger WifiConnectionManager::LOG = Logger("wifi");

WifiConnectionManager::WifiConnectionManager(const StateChangedCallback& callback) :
        state(State::DISCONNECTED, callback) {
    this->reconnectTimer.initializeMs(2000, std::bind(&WifiConnectionManager::connect, this));

    WifiEvents.onStationGotIP(StationGotIPDelegate(&WifiConnectionManager::onStationConfigured, this));
    WifiEvents.onStationConnect(StationConnectDelegate(&WifiConnectionManager::onStationConnected, this));
    WifiEvents.onStationDisconnect(StationDisconnectDelegate(&WifiConnectionManager::onStationDisconnected, this));

    LOG.log(F("Initalized"));
}

WifiConnectionManager::~WifiConnectionManager() {
}

void WifiConnectionManager::connect() {
    LOG.log(F("Connecting"));

    this->reconnectTimer.stop();

    LOG.log(F("SSID:"), WIFI_SSID);
    LOG.log(F("PW:"), WIFI_PWD);
    WifiStation.config(WIFI_SSID, WIFI_PWD);
    LOG.log(F("Configured"));

    WifiAccessPoint.enable(false);
    LOG.log(F("Wifi Access Point disabled"));

    WifiStation.enable(true);
    LOG.log(F("Wifi Station enabled"));

    this->state.set(State::CONNECTING);
    WifiStation.connect();

    LOG.log(F("Waiting for connection"));
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
    LOG.log(F("Configured:"), ip, mask, gateway);

    if (this->state.set(State::CONNECTED)) {
        this->reconnectTimer.stop();
    }
}

void WifiConnectionManager::onStationConnected(const String& ssid, MacAddress bssid, uint8_t channel) {
    LOG.log(F("Connected"));

    this->ssid = ssid;
    this->bssid = bssid;
}

void WifiConnectionManager::onStationDisconnected(const String& ssid, MacAddress bssid, WifiDisconnectReason reason) {
    LOG.log(F("Disconnected"));

    this->ssid = String();
    this->bssid = MacAddress();

    if (this->state.set(State::DISCONNECTED)) {
        this->reconnectTimer.start();
    }
}
