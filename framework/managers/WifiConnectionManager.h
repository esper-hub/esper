#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>

#include "../util/Logger.h"
#include "../util/Observed.h"


class WifiConnectionManager {
    static const Logger LOG;

    WifiConnectionManager(WifiConnectionManager const&) = delete;
    WifiConnectionManager& operator=(WifiConnectionManager const&) = delete;

public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;

public:
    WifiConnectionManager(const StateChangedCallback& callback);
    ~WifiConnectionManager();

    void connect();

    State getState() const;

private:
    void onStationConfigured(IPAddress ip, IPAddress mask, IPAddress gateway);
    void onStationDisconnected(String ssid, uint8_t ssidLength, uint8_t bssid[6], uint8_t reason);

    Observed<State> state;
    Timer reconnectTimer;
};

#endif
