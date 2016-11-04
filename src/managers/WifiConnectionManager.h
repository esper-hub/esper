#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>

#include "../util/Log.h"
#include "../util/Observed.h"


extern const char WIFI_CONNECTION_MANAGER_LOG_NAME[];

class WifiConnectionManager : private Log<WIFI_CONNECTION_MANAGER_LOG_NAME> {
public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;

private:
    Observed<State> state;
    Timer reconnectTimer;

public:
    WifiConnectionManager(const StateChangedCallback cb) : 
            state(State::DISCONNECTED, cb) {
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

        this->state.set(State::CONNECTING);
    }

    inline State getState() const {
        return this->state;
    }


private:
    void onConnectOk() {
        if (this->state.set(State::CONNECTED)) {
            reconnectTimer.stop();
        }
    }

    void onConnectFail() {
        if (this->state.set(State::DISCONNECTED)) {
            reconnectTimer.initializeMs(2000, TimerDelegate(&WifiConnectionManager::connect, this)).start();
        }
    }

};

#endif
