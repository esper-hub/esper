#ifndef WIFI_CONNECTION_MANAGER_H
#define WIFI_CONNECTION_MANAGER_H

#include <SmingCore/SmingCore.h>

#include "../util/Logger.h"
#include "../util/Observed.h"


class WifiConnectionManager {
    static const Logger LOG;

public:
    enum class State {
        CONNECTED,
        CONNECTING,
        DISCONNECTED
    };

    using StateChangedCallback = Observed<State>::Callback;

public:
    WifiConnectionManager(const StateChangedCallback cb);

    void connect();

    State getState() const;

private:
    void onConnectOk();
    void onConnectFail();

    Observed<State> state;
    Timer reconnectTimer;
};

#endif
