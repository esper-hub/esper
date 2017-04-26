#ifndef UPDATE_SERVICE_H
#define UPDATE_SERVICE_H

#include "Service.h"


extern const char UPDATE_SERVICE_NAME[];


class UpdateService : public Service<UPDATE_SERVICE_NAME> {
public:
    explicit UpdateService(const uint16_t& interval);
    virtual ~UpdateService();

protected:
    virtual void initialize(MessageDispatcher* const dispatcher,
                            MessagePublisher* const publisher);

private:
    void triggerUpdate();

    void onTimeout();
    void onMessage(const String& topic, const String& message);

    void onCheckCompleted(HttpClient& client, bool successful);

    HttpClient checkHttpClient;

    rBootHttpUpdate* currentUpdater;

    Timer timer;
};


#endif
