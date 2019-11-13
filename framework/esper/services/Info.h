#ifndef INFO_H
#define INFO_H

#include "Service.h"

extern const char INFO_NAME[];

class Info : public Service<INFO_NAME> {
public:
    Info(Device* const device);
    virtual ~Info();

    virtual void onStateChanged(const State& state);

private:
    String dump() const;

    void publish();

#ifdef INFO_HTTP_ENABLED
    void onHttpIndex(HttpRequest &request, HttpResponse &response);
#endif

    Timer timer;

    uint32_t startupTime;
    uint32_t connectTime;

#ifdef INFO_HTTP_ENABLED
    HttpServer http;
#endif
};


#endif
