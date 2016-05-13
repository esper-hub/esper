#ifndef WIFICONNECTION_MANAGER_H
#define WIFICONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>
#include "../util/Log.h"

extern const char WIFI_CONNECTION_MANAGER_LOG_NAME[];

class WifiConnectionManager : public Log<WIFI_CONNECTION_MANAGER_LOG_NAME> {
public:
    enum class WifiState {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };
    typedef Delegate<void(WifiState)> WifiConnectionStateChangedDelegate;

private:
    Timer reconnectTimer;
    WifiConnectionStateChangedDelegate callback;
    WifiState state;
public:
    WifiConnectionManager(const WifiConnectionStateChangedDelegate cb) : callback(cb),
                                                                         state(WifiState::DISCONNECTED) {
        log("initalized.");
    }

    void connect() {
        reconnectTimer.stop();
        log("configuring Wifi");
        log("Wifi SSID: ", WIFI_SSID);
        log("Wifi PW: ", WIFI_PWD);
        WifiStation.config(WIFI_SSID, WIFI_PWD);
        log("Wifi Station enable: true");
        WifiStation.enable(true);
        log("Wifi Access Point enable: false");
        WifiAccessPoint.enable(false);

        log("Registered waitConnection handlers");
        WifiStation.waitConnection(
                ConnectionDelegate(&WifiConnectionManager::onConnectOk, this), 10,
                ConnectionDelegate(&WifiConnectionManager::onConnectFail, this));
        setState(WifiState::CONNECTING);
    }

    inline WifiState getState() const {
        return state;
    }


private:
    void setState(WifiState state) {
        this->state = state;
        callback(state);
    }

    void onConnectOk() {
        if (getState() != WifiState::CONNECTED) {
            setState(WifiState::CONNECTED);
            reconnectTimer.stop();
        }
    }

    void onConnectFail() {
        if (getState() != WifiState::DISCONNECTED) {
            setState(WifiState::DISCONNECTED);
            reconnectTimer.initializeMs(2000, TimerDelegate(&WifiConnectionManager::connect, this)).start();
        }
    }

};

#endif
