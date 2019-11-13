#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <SmingCore.h>

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

    const String& getCurrentSSID() const;
    const MacAddress& getCurrentBSSID() const;

private:
    void onStationConfigured(IpAddress ip, IpAddress mask, IpAddress gateway);
    void onStationConnected(const String& ssid, MacAddress bssid, uint8_t channel);
    void onStationDisconnected(const String& ssid, MacAddress bssid, WifiDisconnectReason reason);

    Observed<State> state;
    Timer reconnectTimer;

    String ssid;
    MacAddress bssid;
};

#endif
