#ifndef HEARTBEAT_SERVICE_H
#define HEARTBEAT_SERVICE_H

#include "Service.h"


extern const char HEARTBEAT_SERVICE_NAME[];


class HeartbeatService : public Service<HEARTBEAT_SERVICE_NAME> {
    using Service<HEARTBEAT_SERVICE_NAME>::LOG;

public:
    explicit HeartbeatService(const uint16_t& interval);
    virtual ~HeartbeatService();

protected:
    virtual void initialize(MessageDispatcher* const dispatcher,
                            MessagePublisher* const publisher);
    virtual void onConnected();
    virtual void onDisconnected();

private:
    void onTimeout();
    void onMessage(const String& topic, const String& message);

    Timer timer;
};


#endif
